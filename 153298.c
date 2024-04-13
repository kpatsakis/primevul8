dns_message_pseudosectiontotext(dns_message_t *msg,
				dns_pseudosection_t section,
				const dns_master_style_t *style,
				dns_messagetextflag_t flags,
				isc_buffer_t *target)
{
	dns_rdataset_t *ps = NULL;
	dns_name_t *name = NULL;
	isc_result_t result;
	char buf[sizeof("1234567890 ")];
	uint32_t mbz;
	dns_rdata_t rdata;
	isc_buffer_t optbuf;
	uint16_t optcode, optlen;
	unsigned char *optdata;

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(target != NULL);
	REQUIRE(VALID_PSEUDOSECTION(section));

	if ((dns_master_styleflags(style) & DNS_STYLEFLAG_YAML) != 0)
		return (dns_message_pseudosectiontoyaml(msg, section, style,
							flags, target));
	switch (section) {
	case DNS_PSEUDOSECTION_OPT:
		ps = dns_message_getopt(msg);
		if (ps == NULL)
			return (ISC_R_SUCCESS);
		if ((flags & DNS_MESSAGETEXTFLAG_NOCOMMENTS) == 0) {
			INDENT(style);
			ADD_STRING(target, ";; OPT PSEUDOSECTION:\n");
		}

		INDENT(style);
		ADD_STRING(target, "; EDNS: version: ");
		snprintf(buf, sizeof(buf), "%u",
			 (unsigned int)((ps->ttl & 0x00ff0000) >> 16));
		ADD_STRING(target, buf);
		ADD_STRING(target, ", flags:");
		if ((ps->ttl & DNS_MESSAGEEXTFLAG_DO) != 0)
			ADD_STRING(target, " do");
		mbz = ps->ttl & 0xffff;
		mbz &= ~DNS_MESSAGEEXTFLAG_DO;		/* Known Flags. */
		if (mbz != 0) {
			ADD_STRING(target, "; MBZ: ");
			snprintf(buf, sizeof(buf), "0x%.4x", mbz);
			ADD_STRING(target, buf);
			ADD_STRING(target, ", udp: ");
		} else
			ADD_STRING(target, "; udp: ");
		snprintf(buf, sizeof(buf), "%u\n", (unsigned int)ps->rdclass);
		ADD_STRING(target, buf);

		result = dns_rdataset_first(ps);
		if (result != ISC_R_SUCCESS)
			return (ISC_R_SUCCESS);

		/*
		 * Print EDNS info, if any.
		 *
		 * WARNING: The option contents may be malformed as
		 * dig +ednsopt=value:<content> does not validity
		 * checking.
		 */
		dns_rdata_init(&rdata);
		dns_rdataset_current(ps, &rdata);

		isc_buffer_init(&optbuf, rdata.data, rdata.length);
		isc_buffer_add(&optbuf, rdata.length);
		while (isc_buffer_remaininglength(&optbuf) != 0) {
			INSIST(isc_buffer_remaininglength(&optbuf) >= 4U);
			optcode = isc_buffer_getuint16(&optbuf);
			optlen = isc_buffer_getuint16(&optbuf);
			INSIST(isc_buffer_remaininglength(&optbuf) >= optlen);

			INDENT(style);

			if (optcode == DNS_OPT_LLQ) {
				if (optlen == 18U) {
					ADD_STRING(target, "; LLQ:");
					result = render_llq(&optbuf, target);
					if (result != ISC_R_SUCCESS) {
						return (result);
					}
					ADD_STRING(target, "\n");
					continue;
				}
				ADD_STRING(target, "; LLQ");
			} else if (optcode == DNS_OPT_NSID) {
				ADD_STRING(target, "; NSID");
			} else if (optcode == DNS_OPT_COOKIE) {
				ADD_STRING(target, "; COOKIE");
			} else if (optcode == DNS_OPT_CLIENT_SUBNET) {
				isc_buffer_t ecsbuf;

				ADD_STRING(target, "; CLIENT-SUBNET");
				isc_buffer_init(&ecsbuf,
						isc_buffer_current(&optbuf),
						optlen);
				isc_buffer_add(&ecsbuf, optlen);
				result = render_ecs(&ecsbuf, target);
				if (result == ISC_R_NOSPACE)
					return (result);
				if (result == ISC_R_SUCCESS) {
					isc_buffer_forward(&optbuf, optlen);
					ADD_STRING(target, "\n");
					continue;
				}
			} else if (optcode == DNS_OPT_EXPIRE) {
				if (optlen == 4) {
					uint32_t secs;
					secs = isc_buffer_getuint32(&optbuf);
					ADD_STRING(target, "; EXPIRE: ");
					snprintf(buf, sizeof(buf), "%u", secs);
					ADD_STRING(target, buf);
					ADD_STRING(target, " (");
					result = dns_ttl_totext(secs,
								true,
								target);
					if (result != ISC_R_SUCCESS)
						return (result);
					ADD_STRING(target, ")\n");
					continue;
				}
				ADD_STRING(target, "; EXPIRE");
			} else if (optcode == DNS_OPT_PAD) {
				ADD_STRING(target, "; PAD");
			} else if (optcode == DNS_OPT_KEY_TAG) {
				ADD_STRING(target, "; KEY-TAG");
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
				ADD_STRING(target, "; EDE");
				if (optlen >= 2U) {
					uint16_t ede;
					ede = isc_buffer_getuint16(&optbuf);
					snprintf(buf, sizeof(buf), ": %u", ede);
					ADD_STRING(target, buf);
					if (ede < ARRAY_SIZE(edetext)) {
						ADD_STRING(target, " (");
						ADD_STRING(target,
							   edetext[ede]);
						ADD_STRING(target, ")");
					}
					optlen -= 2;
				} else if (optlen == 1U) {
					/* Malformed */
					optdata = isc_buffer_current(&optbuf);
					snprintf(buf, sizeof(buf),
						 ": %02x (\"%c\")\n",
						 optdata[0],
						 isprint(optdata[0])
							 ? optdata[0]
							 : '.');
					isc_buffer_forward(&optbuf, optlen);
					ADD_STRING(target, buf);
					continue;
				}
			} else if (optcode == DNS_OPT_CLIENT_TAG) {
				uint16_t id;
				ADD_STRING(target, "; CLIENT-TAG");
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
				ADD_STRING(target, "; SERVER-TAG");
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
				ADD_STRING(target, "; OPT=");
				snprintf(buf, sizeof(buf), "%u", optcode);
				ADD_STRING(target, buf);
			}

			if (optlen != 0) {
				int i;
				bool utf8ok = false;
				ADD_STRING(target, ": ");

				optdata = isc_buffer_current(&optbuf);
				if (optcode == DNS_OPT_EDE) {
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
				 * version.
				 */
				if (optcode != DNS_OPT_EDE) {
					ADD_STRING(target, "(\"");
				} else {
					ADD_STRING(target, "(");
				}
				if (isc_buffer_availablelength(target) < optlen)
					return (ISC_R_NOSPACE);
				for (i = 0; i < optlen; i++) {
					if (isprint(optdata[i]) ||
					    (utf8ok && optdata[i] > 127)) {
						isc_buffer_putmem(
							target, &optdata[i], 1);
					} else {
						isc_buffer_putstr(target, ".");
					}
				}
				if (optcode != DNS_OPT_EDE) {
					ADD_STRING(target, "\")");
				} else {
					ADD_STRING(target, ")");
				}
			}
			ADD_STRING(target, "\n");
		}
		return (ISC_R_SUCCESS);
	case DNS_PSEUDOSECTION_TSIG:
		ps = dns_message_gettsig(msg, &name);
		if (ps == NULL)
			return (ISC_R_SUCCESS);
		INDENT(style);
		if ((flags & DNS_MESSAGETEXTFLAG_NOCOMMENTS) == 0)
			ADD_STRING(target, ";; TSIG PSEUDOSECTION:\n");
		result = dns_master_rdatasettotext(name, ps, style, target);
		if ((flags & DNS_MESSAGETEXTFLAG_NOHEADERS) == 0 &&
		    (flags & DNS_MESSAGETEXTFLAG_NOCOMMENTS) == 0)
			ADD_STRING(target, "\n");
		return (result);
	case DNS_PSEUDOSECTION_SIG0:
		ps = dns_message_getsig0(msg, &name);
		if (ps == NULL)
			return (ISC_R_SUCCESS);
		INDENT(style);
		if ((flags & DNS_MESSAGETEXTFLAG_NOCOMMENTS) == 0)
			ADD_STRING(target, ";; SIG0 PSEUDOSECTION:\n");
		result = dns_master_rdatasettotext(name, ps, style, target);
		if ((flags & DNS_MESSAGETEXTFLAG_NOHEADERS) == 0 &&
		    (flags & DNS_MESSAGETEXTFLAG_NOCOMMENTS) == 0)
			ADD_STRING(target, "\n");
		return (result);
	}
	result = ISC_R_UNEXPECTED;
 cleanup:
	return (result);
}