getsection(isc_buffer_t *source, dns_message_t *msg, dns_decompress_t *dctx,
	   dns_section_t sectionid, unsigned int options)
{
	isc_region_t r;
	unsigned int count, rdatalen;
	dns_name_t *name = NULL;
	dns_name_t *name2 = NULL;
	dns_offsets_t *offsets;
	dns_rdataset_t *rdataset;
	dns_rdatalist_t *rdatalist;
	isc_result_t result;
	dns_rdatatype_t rdtype, covers;
	dns_rdataclass_t rdclass;
	dns_rdata_t *rdata;
	dns_ttl_t ttl;
	dns_namelist_t *section;
	bool free_name = false, free_rdataset = false;
	bool preserve_order, best_effort, seen_problem;
	bool isedns, issigzero, istsig;

	preserve_order = ((options & DNS_MESSAGEPARSE_PRESERVEORDER) != 0);
	best_effort = ((options & DNS_MESSAGEPARSE_BESTEFFORT) != 0);
	seen_problem = false;

	section = &msg->sections[sectionid];

	for (count = 0; count < msg->counts[sectionid]; count++) {
		int recstart = source->current;
		bool skip_name_search, skip_type_search;

		skip_name_search = false;
		skip_type_search = false;
		free_rdataset = false;
		isedns = false;
		issigzero = false;
		istsig = false;

		name = isc_mempool_get(msg->namepool);
		if (name == NULL)
			return (ISC_R_NOMEMORY);
		free_name = true;

		offsets = newoffsets(msg);
		if (offsets == NULL) {
			result = ISC_R_NOMEMORY;
			goto cleanup;
		}
		dns_name_init(name, *offsets);

		/*
		 * Parse the name out of this packet.
		 */
		isc_buffer_remainingregion(source, &r);
		isc_buffer_setactive(source, r.length);
		result = getname(name, source, msg, dctx);
		if (result != ISC_R_SUCCESS)
			goto cleanup;

		/*
		 * Get type, class, ttl, and rdatalen.  Verify that at least
		 * rdatalen bytes remain.  (Some of this is deferred to
		 * later.)
		 */
		isc_buffer_remainingregion(source, &r);
		if (r.length < 2 + 2 + 4 + 2) {
			result = ISC_R_UNEXPECTEDEND;
			goto cleanup;
		}
		rdtype = isc_buffer_getuint16(source);
		rdclass = isc_buffer_getuint16(source);

		/*
		 * If there was no question section, we may not yet have
		 * established a class.  Do so now.
		 */
		if (msg->rdclass_set == 0 &&
		    rdtype != dns_rdatatype_opt &&	/* class is UDP SIZE */
		    rdtype != dns_rdatatype_tsig &&	/* class is ANY */
		    rdtype != dns_rdatatype_tkey) {	/* class is undefined */
			msg->rdclass = rdclass;
			msg->rdclass_set = 1;
		}

		/*
		 * If this class is different than the one in the question
		 * section, bail.
		 */
		if (msg->opcode != dns_opcode_update
		    && rdtype != dns_rdatatype_tsig
		    && rdtype != dns_rdatatype_opt
		    && rdtype != dns_rdatatype_key /* in a TKEY query */
		    && rdtype != dns_rdatatype_sig /* SIG(0) */
		    && rdtype != dns_rdatatype_tkey /* Win2000 TKEY */
		    && msg->rdclass != dns_rdataclass_any
		    && msg->rdclass != rdclass)
			DO_ERROR(DNS_R_FORMERR);

		/*
		 * If this is not a TKEY query/response then the KEY
		 * record's class needs to match.
		 */
		if (msg->opcode != dns_opcode_update && !msg->tkey &&
		    rdtype == dns_rdatatype_key &&
		    msg->rdclass != dns_rdataclass_any &&
		    msg->rdclass != rdclass)
			DO_ERROR(DNS_R_FORMERR);

		/*
		 * Special type handling for TSIG, OPT, and TKEY.
		 */
		if (rdtype == dns_rdatatype_tsig) {
			/*
			 * If it is a tsig, verify that it is in the
			 * additional data section.
			 */
			if (sectionid != DNS_SECTION_ADDITIONAL ||
			    rdclass != dns_rdataclass_any ||
			    count != msg->counts[sectionid]  - 1) {
				DO_ERROR(DNS_R_BADTSIG);
			} else {
				skip_name_search = true;
				skip_type_search = true;
				istsig = true;
			}
		} else if (rdtype == dns_rdatatype_opt) {
			/*
			 * The name of an OPT record must be ".", it
			 * must be in the additional data section, and
			 * it must be the first OPT we've seen.
			 */
			if (!dns_name_equal(dns_rootname, name) ||
			    sectionid != DNS_SECTION_ADDITIONAL ||
			    msg->opt != NULL) {
				DO_ERROR(DNS_R_FORMERR);
			} else {
				skip_name_search = true;
				skip_type_search = true;
				isedns = true;
			}
		} else if (rdtype == dns_rdatatype_tkey) {
			/*
			 * A TKEY must be in the additional section if this
			 * is a query, and the answer section if this is a
			 * response.  Unless it's a Win2000 client.
			 *
			 * Its class is ignored.
			 */
			dns_section_t tkeysection;

			if ((msg->flags & DNS_MESSAGEFLAG_QR) == 0)
				tkeysection = DNS_SECTION_ADDITIONAL;
			else
				tkeysection = DNS_SECTION_ANSWER;
			if (sectionid != tkeysection &&
			    sectionid != DNS_SECTION_ANSWER)
				DO_ERROR(DNS_R_FORMERR);
		}

		/*
		 * ... now get ttl and rdatalen, and check buffer.
		 */
		ttl = isc_buffer_getuint32(source);
		rdatalen = isc_buffer_getuint16(source);
		r.length -= (2 + 2 + 4 + 2);
		if (r.length < rdatalen) {
			result = ISC_R_UNEXPECTEDEND;
			goto cleanup;
		}

		/*
		 * Read the rdata from the wire format.  Interpret the
		 * rdata according to its actual class, even if it had a
		 * DynDNS meta-class in the packet (unless this is a TSIG).
		 * Then put the meta-class back into the finished rdata.
		 */
		rdata = newrdata(msg);
		if (rdata == NULL) {
			result = ISC_R_NOMEMORY;
			goto cleanup;
		}
		if (msg->opcode == dns_opcode_update &&
		    update(sectionid, rdclass)) {
			if (rdatalen != 0) {
				result = DNS_R_FORMERR;
				goto cleanup;
			}
			/*
			 * When the rdata is empty, the data pointer is
			 * never dereferenced, but it must still be non-NULL.
			 * Casting 1 rather than "" avoids warnings about
			 * discarding the const attribute of a string,
			 * for compilers that would warn about such things.
			 */
			rdata->data = (unsigned char *)1;
			rdata->length = 0;
			rdata->rdclass = rdclass;
			rdata->type = rdtype;
			rdata->flags = DNS_RDATA_UPDATE;
			result = ISC_R_SUCCESS;
		} else if (rdclass == dns_rdataclass_none &&
			   msg->opcode == dns_opcode_update &&
			   sectionid == DNS_SECTION_UPDATE) {
			result = getrdata(source, msg, dctx, msg->rdclass,
					  rdtype, rdatalen, rdata);
		} else
			result = getrdata(source, msg, dctx, rdclass,
					  rdtype, rdatalen, rdata);
		if (result != ISC_R_SUCCESS)
			goto cleanup;
		rdata->rdclass = rdclass;
		if (rdtype == dns_rdatatype_rrsig &&
		    rdata->flags == 0) {
			covers = dns_rdata_covers(rdata);
			if (covers == 0)
				DO_ERROR(DNS_R_FORMERR);
		} else if (rdtype == dns_rdatatype_sig /* SIG(0) */ &&
			   rdata->flags == 0) {
			covers = dns_rdata_covers(rdata);
			if (covers == 0) {
				if (sectionid != DNS_SECTION_ADDITIONAL ||
				    count != msg->counts[sectionid]  - 1) {
					DO_ERROR(DNS_R_BADSIG0);
				} else {
					skip_name_search = true;
					skip_type_search = true;
					issigzero = true;
				}
			} else {
				if (msg->rdclass != dns_rdataclass_any &&
				    msg->rdclass != rdclass)
					DO_ERROR(DNS_R_FORMERR);
			}
		} else
			covers = 0;

		/*
		 * Check the ownername of NSEC3 records
		 */
		if (rdtype == dns_rdatatype_nsec3 &&
		    !dns_rdata_checkowner(name, msg->rdclass, rdtype,
					  false)) {
			result = DNS_R_BADOWNERNAME;
			goto cleanup;
		}

		/*
		 * If we are doing a dynamic update or this is a meta-type,
		 * don't bother searching for a name, just append this one
		 * to the end of the message.
		 */
		if (preserve_order || msg->opcode == dns_opcode_update ||
		    skip_name_search) {
			if (!isedns && !istsig && !issigzero) {
				ISC_LIST_APPEND(*section, name, link);
				free_name = false;
			}
		} else {
			/*
			 * Run through the section, looking to see if this name
			 * is already there.  If it is found, put back the
			 * allocated name since we no longer need it, and set
			 * our name pointer to point to the name we found.
			 */
			result = findname(&name2, name, section);

			/*
			 * If it is a new name, append to the section.
			 */
			if (result == ISC_R_SUCCESS) {
				isc_mempool_put(msg->namepool, name);
				name = name2;
			} else {
				ISC_LIST_APPEND(*section, name, link);
			}
			free_name = false;
		}

		/*
		 * Search name for the particular type and class.
		 * Skip this stage if in update mode or this is a meta-type.
		 */
		if (preserve_order || msg->opcode == dns_opcode_update ||
		    skip_type_search)
			result = ISC_R_NOTFOUND;
		else {
			/*
			 * If this is a type that can only occur in
			 * the question section, fail.
			 */
			if (dns_rdatatype_questiononly(rdtype))
				DO_ERROR(DNS_R_FORMERR);

			rdataset = NULL;
			result = dns_message_find(name, rdclass, rdtype,
						   covers, &rdataset);
		}

		/*
		 * If we found an rdataset that matches, we need to
		 * append this rdata to that set.  If we did not, we need
		 * to create a new rdatalist, store the important bits there,
		 * convert it to an rdataset, and link the latter to the name.
		 * Yuck.  When appending, make certain that the type isn't
		 * a singleton type, such as SOA or CNAME.
		 *
		 * Note that this check will be bypassed when preserving order,
		 * the opcode is an update, or the type search is skipped.
		 */
		if (result == ISC_R_SUCCESS) {
			if (dns_rdatatype_issingleton(rdtype)) {
				dns_rdata_t *first;
				dns_rdatalist_fromrdataset(rdataset,
							   &rdatalist);
				first = ISC_LIST_HEAD(rdatalist->rdata);
				INSIST(first != NULL);
				if (dns_rdata_compare(rdata, first) != 0)
					DO_ERROR(DNS_R_FORMERR);
			}
		}

		if (result == ISC_R_NOTFOUND) {
			rdataset = isc_mempool_get(msg->rdspool);
			if (rdataset == NULL) {
				result = ISC_R_NOMEMORY;
				goto cleanup;
			}
			free_rdataset = true;

			rdatalist = newrdatalist(msg);
			if (rdatalist == NULL) {
				result = ISC_R_NOMEMORY;
				goto cleanup;
			}

			rdatalist->type = rdtype;
			rdatalist->covers = covers;
			rdatalist->rdclass = rdclass;
			rdatalist->ttl = ttl;

			dns_rdataset_init(rdataset);
			RUNTIME_CHECK(dns_rdatalist_tordataset(rdatalist,
							       rdataset)
				      == ISC_R_SUCCESS);
			dns_rdataset_setownercase(rdataset, name);

			if (!isedns && !istsig && !issigzero) {
				ISC_LIST_APPEND(name->list, rdataset, link);
				free_rdataset = false;
			}
		}

		/*
		 * Minimize TTLs.
		 *
		 * Section 5.2 of RFC2181 says we should drop
		 * nonauthoritative rrsets where the TTLs differ, but we
		 * currently treat them the as if they were authoritative and
		 * minimize them.
		 */
		if (ttl != rdataset->ttl) {
			rdataset->attributes |= DNS_RDATASETATTR_TTLADJUSTED;
			if (ttl < rdataset->ttl)
				rdataset->ttl = ttl;
		}

		/* Append this rdata to the rdataset. */
		dns_rdatalist_fromrdataset(rdataset, &rdatalist);
		ISC_LIST_APPEND(rdatalist->rdata, rdata, link);

		/*
		 * If this is an OPT, SIG(0) or TSIG record, remember it.
		 * Also, set the extended rcode for TSIG.
		 *
		 * Note msg->opt, msg->sig0 and msg->tsig will only be
		 * already set if best-effort parsing is enabled otherwise
		 * there will only be at most one of each.
		 */
		if (isedns) {
			dns_rcode_t ercode;

			msg->opt = rdataset;
			rdataset = NULL;
			free_rdataset = false;
			ercode = (dns_rcode_t)
				((msg->opt->ttl & DNS_MESSAGE_EDNSRCODE_MASK)
				 >> 20);
			msg->rcode |= ercode;
			isc_mempool_put(msg->namepool, name);
			free_name = false;
		} else if (issigzero) {
			msg->sig0 = rdataset;
			msg->sig0name = name;
			msg->sigstart = recstart;
			rdataset = NULL;
			free_rdataset = false;
			free_name = false;
		} else if (istsig) {
			msg->tsig = rdataset;
			msg->tsigname = name;
			msg->sigstart = recstart;
			/*
			 * Windows doesn't like TSIG names to be compressed.
			 */
			msg->tsigname->attributes |= DNS_NAMEATTR_NOCOMPRESS;
			rdataset = NULL;
			free_rdataset = false;
			free_name = false;
		}

		if (seen_problem) {
			if (free_name)
				isc_mempool_put(msg->namepool, name);
			if (free_rdataset)
				isc_mempool_put(msg->rdspool, rdataset);
			free_name = free_rdataset = false;
		}
		INSIST(free_name == false);
		INSIST(free_rdataset == false);
	}

	/*
	 * If any of DS, NSEC or NSEC3 appeared in the
	 * authority section of a query response without
	 * a covering RRSIG, FORMERR
	 */
	if (sectionid == DNS_SECTION_AUTHORITY &&
	    msg->opcode == dns_opcode_query &&
	    ((msg->flags & DNS_MESSAGEFLAG_QR) != 0) &&
	    ((msg->flags & DNS_MESSAGEFLAG_TC) == 0) &&
	    !preserve_order &&
	    !auth_signed(section))
		DO_ERROR(DNS_R_FORMERR);

	if (seen_problem)
		return (DNS_R_RECOVERABLE);
	return (ISC_R_SUCCESS);

 cleanup:
	if (free_name)
		isc_mempool_put(msg->namepool, name);
	if (free_rdataset)
		isc_mempool_put(msg->rdspool, rdataset);

	return (result);
}