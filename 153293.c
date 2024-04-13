dns_message_pseudosectiontoyaml(dns_message_t *msg,
				dns_pseudosection_t section,
				const dns_master_style_t *style,
				dns_messagetextflag_t flags,
				isc_buffer_t *target)
{
	dns_rdataset_t *ps = NULL;
	dns_name_t *name = NULL;
	isc_result_t result = ISC_R_SUCCESS;
	char buf[sizeof("1234567890")];
	uint32_t mbz;
	dns_rdata_t rdata;
	isc_buffer_t optbuf;
	uint16_t optcode, optlen;
	unsigned char *optdata;
	unsigned int saveindent = dns_master_indent;
	unsigned int optindent;

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(target != NULL);
	REQUIRE(VALID_PSEUDOSECTION(section));

	switch (section) {
	case DNS_PSEUDOSECTION_OPT:
		ps = dns_message_getopt(msg);
		if (ps == NULL) {
			goto cleanup;
		}

		INDENT(style);
		ADD_STRING(target, "OPT_PSEUDOSECTION:\n");
		dns_master_indent++;

		INDENT(style);
		ADD_STRING(target, "EDNS:\n");
		dns_master_indent++;

		INDENT(style);
		ADD_STRING(target, "version: ");
		snprintf(buf, sizeof(buf), "%u",
			 (unsigned int)((ps->ttl & 0x00ff0000) >> 16));
		ADD_STRING(target, buf);
		ADD_STRING(target, "\n");
		INDENT(style);
		ADD_STRING(target, "flags:");
		if ((ps->ttl & DNS_MESSAGEEXTFLAG_DO) != 0)
			ADD_STRING(target, " do");
		ADD_STRING(target, "\n");
		mbz = ps->ttl & 0xffff;
		mbz &= ~DNS_MESSAGEEXTFLAG_DO;		/* Known Flags. */
		if (mbz != 0) {
			INDENT(style);
			ADD_STRING(target, "MBZ: ");
			snprintf(buf, sizeof(buf), "0x%.4x", mbz);
			ADD_STRING(target, buf);
			ADD_STRING(target, "\n");
		}
		INDENT(style);
		ADD_STRING(target, "udp: ");
		snprintf(buf, sizeof(buf), "%u\n", (unsigned int)ps->rdclass);
		ADD_STRING(target, buf);
		result = dns_rdataset_first(ps);
		if (result != ISC_R_SUCCESS) {
			result = ISC_R_SUCCESS;
			goto cleanup;
		}

		/*
		 * Print EDNS info, if any.
		 *
		 * WARNING: The option contents may be malformed as
		 * dig +ednsopt=value:<content> does not perform validity
		 * checking.
		 */
		dns_rdata_init(&rdata);
		dns_rdataset_current(ps, &rdata);

		isc_buffer_init(&optbuf, rdata.data, rdata.length);
		isc_buffer_add(&optbuf, rdata.length);
		optindent = dns_master_indent;
		while (isc_buffer_remaininglength(&optbuf) != 0) {
			bool extra_text = false;
			dns_master_indent = optindent;
			INSIST(isc_buffer_remaininglength(&optbuf) >= 4U);
			optcode = isc_buffer_getuint16(&optbuf);
			optlen = isc_buffer_getuint16(&optbuf);
			INSIST(isc_buffer_remaininglength(&optbuf) >= optlen);

			if (optcode == DNS_OPT_LLQ) {
				INDENT(style);
				if (optlen == 18U) {
					ADD_STRING(target, "LLQ: ");
					result = render_llq(&optbuf, target);
					if (result != ISC_R_SUCCESS) {
						goto cleanup;
					}
					ADD_STRING(target, "\n");
					continue;
				}
				ADD_STRING(target, "LLQ");
			} else if (optcode == DNS_OPT_NSID) {
				INDENT(style);
				ADD_STRING(target, "NSID");
			} else if (optcode == DNS_OPT_COOKIE) {
				INDENT(style);
				ADD_STRING(target, "COOKIE");
			} else if (optcode == DNS_OPT_CLIENT_SUBNET) {
				isc_buffer_t ecsbuf;
				INDENT(style);
				ADD_STRING(target, "CLIENT-SUBNET");
				isc_buffer_init(&ecsbuf,
						isc_buffer_current(&optbuf),
						optlen);
				isc_buffer_add(&ecsbuf, optlen);
				result = render_ecs(&ecsbuf, target);
				if (result == ISC_R_NOSPACE)
					goto cleanup;
				if (result == ISC_R_SUCCESS) {
					isc_buffer_forward(&optbuf, optlen);
					ADD_STRING(target, "\n");
					continue;
				}
				ADD_STRING(target, "\n");
			} else if (optcode == DNS_OPT_EXPIRE) {
				if (optlen == 4) {
					uint32_t secs;
					secs = isc_buffer_getuint32(&optbuf);
					INDENT(style);
					ADD_STRING(target, "EXPIRE: ");
					snprintf(buf, sizeof(buf), "%u", secs);
					ADD_STRING(target, buf);
					ADD_STRING(target, " (");
					result = dns_ttl_totext(secs,
								true,
								target);
					if (result != ISC_R_SUCCESS)
						goto cleanup;
					ADD_STRING(target, ")\n");
					continue;
				}
				INDENT(style);
				ADD_STRING(target, "EXPIRE");
			} else if (optcode == DNS_OPT_PAD) {
				INDENT(style);
				ADD_STRING(target, "PAD");
			} else if (optcode == DNS_OPT_KEY_TAG) {
				INDENT(style);
				ADD_STRING(target, "KEY-TAG");
				if (optlen > 0U && (optlen % 2U) == 0U) {
					const char *sep = ": ";
					uint16_t id;
					while (optlen > 0U) {
					    id = isc_buffer_getuint16(&optbuf);
					    snprintf(buf, sizeof(buf), "%s%u",
						     sep, id);
					    ADD_STRING(target, buf);
					    sep = ", ";
					    optlen -= 2;
					}
					ADD_STRING(target, "\n");
					continue;
				}
			} else if (optcode == DNS_OPT_EDE) {
				INDENT(style);
				ADD_STRING(target, "EDE");
				if (optlen >= 2U) {
					uint16_t ede;
					ADD_STRING(target, ":\n");
					dns_master_indent++;
					INDENT(style);
					ADD_STRING(target, "INFO-CODE:");
					ede = isc_buffer_getuint16(&optbuf);
					snprintf(buf, sizeof(buf), " %u", ede);
					ADD_STRING(target, buf);
					if (ede < ARRAY_SIZE(edetext)) {
						ADD_STRING(target, " (");
						ADD_STRING(target,
							   edetext[ede]);
						ADD_STRING(target, ")");
					}
					ADD_STRING(target, "\n");
					optlen -= 2;
					if (optlen != 0) {
						INDENT(style);
						ADD_STRING(target,
							   "EXTRA-TEXT");
						extra_text = true;
					}
				}
			} else if (optcode == DNS_OPT_CLIENT_TAG) {
				uint16_t id;
				INDENT(style);
				ADD_STRING(target, "CLIENT-TAG");
				if (optlen == 2U) {
					id = isc_buffer_getuint16(&optbuf);
					snprintf(buf, sizeof(buf), ": %u\n",
						 id);
					ADD_STRING(target, buf);
					optlen -= 2;
					POST(optlen);
					continue;
				}
			} else if (optcode == DNS_OPT_SERVER_TAG) {
				uint16_t id;
				INDENT(style);
				ADD_STRING(target, "SERVER-TAG");
				if (optlen == 2U) {
					id = isc_buffer_getuint16(&optbuf);
					snprintf(buf, sizeof(buf), ": %u\n",
						 id);
					ADD_STRING(target, buf);
					optlen -= 2;
					POST(optlen);
					continue;
				}
			} else {
				INDENT(style);
				ADD_STRING(target, "OPT: ");
				snprintf(buf, sizeof(buf), "%u", optcode);
				ADD_STRING(target, buf);
				ADD_STRING(target, "\n");
			}

			if (optlen != 0) {
				int i;
				bool utf8ok = false;

				ADD_STRING(target, ": ");

				optdata = isc_buffer_current(&optbuf);
				if (extra_text) {
					utf8ok = isc_utf8_valid(optdata,
								optlen);
				}
				if (!utf8ok) {
					for (i = 0; i < optlen; i++) {
						const char *sep;
						switch (optcode) {
						case DNS_OPT_COOKIE:
							sep = "";
							break;
						default:
							sep = " ";
							break;
						}
						snprintf(buf, sizeof(buf),
							 "%02x%s", optdata[i],
							 sep);
						ADD_STRING(target, buf);
					}
				}

				isc_buffer_forward(&optbuf, optlen);

				if (optcode == DNS_OPT_COOKIE) {
					/*
					 * Valid server cookie?
					 */
					if (msg->cc_ok && optlen >= 16)
						ADD_STRING(target, " (good)");
					/*
					 * Server cookie is not valid but
					 * we had our cookie echoed back.
					 */
					if (msg->cc_ok && optlen < 16)
						ADD_STRING(target, " (echoed)");
					/*
					 * We didn't get our cookie echoed
					 * back.
					 */
					if (msg->cc_bad)
						ADD_STRING(target, " (bad)");
					ADD_STRING(target, "\n");
					continue;
				}

				if (optcode == DNS_OPT_CLIENT_SUBNET) {
					ADD_STRING(target, "\n");
					continue;
				}

				/*
				 * For non-COOKIE options, add a printable
				 * version
				 */
				if (!extra_text) {
					ADD_STRING(target, "(\"");
				} else {
					ADD_STRING(target, "\"");
				}
				if (isc_buffer_availablelength(target) < optlen)
				{
					result = ISC_R_NOSPACE;
					goto cleanup;
				}
				for (i = 0; i < optlen; i++) {
					if (isprint(optdata[i]) ||
					    (utf8ok && optdata[i] > 127)) {
						isc_buffer_putmem(
							target, &optdata[i], 1);
					} else {
						isc_buffer_putstr(target, ".");
					}
				}
				if (!extra_text) {
					ADD_STRING(target, "\")");
				} else {
					ADD_STRING(target, "\"");
				}
			}
			ADD_STRING(target, "\n");
		}
		dns_master_indent = optindent;
		result = ISC_R_SUCCESS;
		goto cleanup;
	case DNS_PSEUDOSECTION_TSIG:
		ps = dns_message_gettsig(msg, &name);
		if (ps == NULL) {
			result = ISC_R_SUCCESS;
			goto cleanup;
		}
		INDENT(style);
		ADD_STRING(target, "TSIG_PSEUDOSECTION:\n");
		result = dns_master_rdatasettotext(name, ps, style, target);
		ADD_STRING(target, "\n");
		goto cleanup;
	case DNS_PSEUDOSECTION_SIG0:
		ps = dns_message_getsig0(msg, &name);
		if (ps == NULL) {
			result = ISC_R_SUCCESS;
			goto cleanup;
		}
		INDENT(style);
		ADD_STRING(target, "SIG0_PSEUDOSECTION:\n");
		result = dns_master_rdatasettotext(name, ps, style, target);
		if ((flags & DNS_MESSAGETEXTFLAG_NOHEADERS) == 0 &&
		    (flags & DNS_MESSAGETEXTFLAG_NOCOMMENTS) == 0)
			ADD_STRING(target, "\n");
		goto cleanup;
	}

	result = ISC_R_UNEXPECTED;

 cleanup:
	dns_master_indent = saveindent;
	return (result);
}