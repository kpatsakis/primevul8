dns_message_rendersection(dns_message_t *msg, dns_section_t sectionid,
			  unsigned int options)
{
	dns_namelist_t *section;
	dns_name_t *name, *next_name;
	dns_rdataset_t *rdataset, *next_rdataset;
	unsigned int count, total;
	isc_result_t result;
	isc_buffer_t st; /* for rollbacks */
	int pass;
	bool partial = false;
	unsigned int rd_options;
	dns_rdatatype_t preferred_glue = 0;

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(msg->buffer != NULL);
	REQUIRE(VALID_NAMED_SECTION(sectionid));

	section = &msg->sections[sectionid];

	if ((sectionid == DNS_SECTION_ADDITIONAL)
	    && (options & DNS_MESSAGERENDER_ORDERED) == 0) {
		if ((options & DNS_MESSAGERENDER_PREFER_A) != 0) {
			preferred_glue = dns_rdatatype_a;
			pass = 4;
		} else if ((options & DNS_MESSAGERENDER_PREFER_AAAA) != 0) {
			preferred_glue = dns_rdatatype_aaaa;
			pass = 4;
		} else
			pass = 3;
	} else
		pass = 1;

	if ((options & DNS_MESSAGERENDER_OMITDNSSEC) == 0)
		rd_options = 0;
	else
		rd_options = DNS_RDATASETTOWIRE_OMITDNSSEC;

	/*
	 * Shrink the space in the buffer by the reserved amount.
	 */
	if (msg->buffer->length - msg->buffer->used < msg->reserved)
		return (ISC_R_NOSPACE);
	msg->buffer->length -= msg->reserved;

	total = 0;
	if (msg->reserved == 0 && (options & DNS_MESSAGERENDER_PARTIAL) != 0)
		partial = true;

	/*
	 * Render required glue first.  Set TC if it won't fit.
	 */
	name = ISC_LIST_HEAD(*section);
	if (name != NULL) {
		rdataset = ISC_LIST_HEAD(name->list);
		if (rdataset != NULL &&
		    (rdataset->attributes & DNS_RDATASETATTR_REQUIREDGLUE) != 0 &&
		    (rdataset->attributes & DNS_RDATASETATTR_RENDERED) == 0) {
			const void *order_arg = msg->order_arg;
			st = *(msg->buffer);
			count = 0;
			if (partial)
				result = dns_rdataset_towirepartial(rdataset,
								    name,
								    msg->cctx,
								    msg->buffer,
								    msg->order,
								    order_arg,
								    rd_options,
								    &count,
								    NULL);
			else
				result = dns_rdataset_towiresorted(rdataset,
								   name,
								   msg->cctx,
								   msg->buffer,
								   msg->order,
								   order_arg,
								   rd_options,
								   &count);
			total += count;
			if (partial && result == ISC_R_NOSPACE) {
				msg->flags |= DNS_MESSAGEFLAG_TC;
				msg->buffer->length += msg->reserved;
				msg->counts[sectionid] += total;
				return (result);
			}
			if (result == ISC_R_NOSPACE)
				msg->flags |= DNS_MESSAGEFLAG_TC;
			if (result != ISC_R_SUCCESS) {
				INSIST(st.used < 65536);
				dns_compress_rollback(msg->cctx,
						      (uint16_t)st.used);
				*(msg->buffer) = st;  /* rollback */
				msg->buffer->length += msg->reserved;
				msg->counts[sectionid] += total;
				return (result);
			}
			rdataset->attributes |= DNS_RDATASETATTR_RENDERED;
		}
	}

	do {
		name = ISC_LIST_HEAD(*section);
		if (name == NULL) {
			msg->buffer->length += msg->reserved;
			msg->counts[sectionid] += total;
			return (ISC_R_SUCCESS);
		}

		while (name != NULL) {
			next_name = ISC_LIST_NEXT(name, link);

			rdataset = ISC_LIST_HEAD(name->list);
			while (rdataset != NULL) {
				next_rdataset = ISC_LIST_NEXT(rdataset, link);

				if ((rdataset->attributes &
				     DNS_RDATASETATTR_RENDERED) != 0)
					goto next;

				if (((options & DNS_MESSAGERENDER_ORDERED)
				     == 0)
				    && (sectionid == DNS_SECTION_ADDITIONAL)
				    && wrong_priority(rdataset, pass,
						      preferred_glue))
					goto next;

#ifdef ALLOW_FILTER_AAAA
				/*
				 * Suppress AAAAs if asked and we are
				 * not doing DNSSEC or are breaking DNSSEC.
				 * Say so in the AD bit if we break DNSSEC.
				 */
				if (norender_rdataset(rdataset, options, sectionid)) {
					if (sectionid == DNS_SECTION_ANSWER ||
					    sectionid == DNS_SECTION_AUTHORITY)
					    msg->flags &= ~DNS_MESSAGEFLAG_AD;
					if (OPTOUT(rdataset))
					    msg->flags &= ~DNS_MESSAGEFLAG_AD;
					goto next;
				}

#endif
				st = *(msg->buffer);

				count = 0;
				if (partial)
					result = dns_rdataset_towirepartial(
							  rdataset,
							  name,
							  msg->cctx,
							  msg->buffer,
							  msg->order,
							  msg->order_arg,
							  rd_options,
							  &count,
							  NULL);
				else
					result = dns_rdataset_towiresorted(
							  rdataset,
							  name,
							  msg->cctx,
							  msg->buffer,
							  msg->order,
							  msg->order_arg,
							  rd_options,
							  &count);

				total += count;

				/*
				 * If out of space, record stats on what we
				 * rendered so far, and return that status.
				 *
				 * XXXMLG Need to change this when
				 * dns_rdataset_towire() can render partial
				 * sets starting at some arbitrary point in the
				 * set.  This will include setting a bit in the
				 * rdataset to indicate that a partial
				 * rendering was done, and some state saved
				 * somewhere (probably in the message struct)
				 * to indicate where to continue from.
				 */
				if (partial && result == ISC_R_NOSPACE) {
					msg->buffer->length += msg->reserved;
					msg->counts[sectionid] += total;
					return (result);
				}
				if (result != ISC_R_SUCCESS) {
					INSIST(st.used < 65536);
					dns_compress_rollback(msg->cctx,
							(uint16_t)st.used);
					*(msg->buffer) = st;  /* rollback */
					msg->buffer->length += msg->reserved;
					msg->counts[sectionid] += total;
					maybe_clear_ad(msg, sectionid);
					return (result);
				}

				/*
				 * If we have rendered non-validated data,
				 * ensure that the AD bit is not set.
				 */
				if (rdataset->trust != dns_trust_secure &&
				    (sectionid == DNS_SECTION_ANSWER ||
				     sectionid == DNS_SECTION_AUTHORITY))
					msg->flags &= ~DNS_MESSAGEFLAG_AD;
				if (OPTOUT(rdataset))
					msg->flags &= ~DNS_MESSAGEFLAG_AD;

				rdataset->attributes |=
					DNS_RDATASETATTR_RENDERED;

			next:
				rdataset = next_rdataset;
			}

			name = next_name;
		}
	} while (--pass != 0);

	msg->buffer->length += msg->reserved;
	msg->counts[sectionid] += total;

	return (ISC_R_SUCCESS);
}