dns_message_totext(dns_message_t *msg, const dns_master_style_t *style,
		   dns_messagetextflag_t flags, isc_buffer_t *target)
{
	char buf[sizeof("1234567890")];
	isc_result_t result;

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(target != NULL);

	if (((flags & DNS_MESSAGETEXTFLAG_NOHEADERS) == 0) &&
	     (dns_master_styleflags(style) & DNS_STYLEFLAG_YAML))
	{
		INDENT(style);
		ADD_STRING(target, "opcode: ");
		ADD_STRING(target, opcodetext[msg->opcode]);
		ADD_STRING(target, "\n");
		INDENT(style);
		ADD_STRING(target, "status: ");
		result = dns_rcode_totext(msg->rcode, target);
		if (result != ISC_R_SUCCESS)
			return (result);
		ADD_STRING(target, "\n");
		INDENT(style);
		ADD_STRING(target, "id: ");
		snprintf(buf, sizeof(buf), "%6u", msg->id);
		ADD_STRING(target, buf);
		ADD_STRING(target, "\n");
		INDENT(style);
		ADD_STRING(target, "flags:");
		if ((msg->flags & DNS_MESSAGEFLAG_QR) != 0)
			ADD_STRING(target, " qr");
		if ((msg->flags & DNS_MESSAGEFLAG_AA) != 0)
			ADD_STRING(target, " aa");
		if ((msg->flags & DNS_MESSAGEFLAG_TC) != 0)
			ADD_STRING(target, " tc");
		if ((msg->flags & DNS_MESSAGEFLAG_RD) != 0)
			ADD_STRING(target, " rd");
		if ((msg->flags & DNS_MESSAGEFLAG_RA) != 0)
			ADD_STRING(target, " ra");
		if ((msg->flags & DNS_MESSAGEFLAG_AD) != 0)
			ADD_STRING(target, " ad");
		if ((msg->flags & DNS_MESSAGEFLAG_CD) != 0)
			ADD_STRING(target, " cd");
		ADD_STRING(target, "\n");
		/*
		 * The final unnamed flag must be zero.
		 */
		if ((msg->flags & 0x0040U) != 0) {
			INDENT(style);
			ADD_STRING(target, "MBZ: 0x4");
			ADD_STRING(target, "\n");
		}
		if (msg->opcode != dns_opcode_update) {
			INDENT(style);
			ADD_STRING(target, "QUESTION: ");
		} else {
			ADD_STRING(target, "ZONE: ");
		}
		snprintf(buf, sizeof(buf), "%1u",
			 msg->counts[DNS_SECTION_QUESTION]);
		ADD_STRING(target, buf);
		ADD_STRING(target, "\n");
		if (msg->opcode != dns_opcode_update) {
			INDENT(style);
			ADD_STRING(target, "ANSWER: ");
		} else {
			INDENT(style);
			ADD_STRING(target, "PREREQ: ");
		}
		snprintf(buf, sizeof(buf), "%1u",
			 msg->counts[DNS_SECTION_ANSWER]);
		ADD_STRING(target, buf);
		ADD_STRING(target, "\n");
		if (msg->opcode != dns_opcode_update) {
			INDENT(style);
			ADD_STRING(target, "AUTHORITY: ");
		} else {
			INDENT(style);
			ADD_STRING(target, "UPDATE: ");
		}
		snprintf(buf, sizeof(buf), "%1u",
			msg->counts[DNS_SECTION_AUTHORITY]);
		ADD_STRING(target, buf);
		ADD_STRING(target, "\n");
		INDENT(style);
		ADD_STRING(target, "ADDITIONAL: ");
		snprintf(buf, sizeof(buf), "%1u",
			msg->counts[DNS_SECTION_ADDITIONAL]);
		ADD_STRING(target, buf);
		ADD_STRING(target, "\n");
	} else if ((flags & DNS_MESSAGETEXTFLAG_NOHEADERS) == 0) {
		INDENT(style);
		ADD_STRING(target, ";; ->>HEADER<<- opcode: ");
		ADD_STRING(target, opcodetext[msg->opcode]);
		ADD_STRING(target, ", status: ");
		result = dns_rcode_totext(msg->rcode, target);
		if (result != ISC_R_SUCCESS)
			return (result);
		ADD_STRING(target, ", id: ");
		snprintf(buf, sizeof(buf), "%6u", msg->id);
		ADD_STRING(target, buf);
		ADD_STRING(target, "\n");
		INDENT(style);
		ADD_STRING(target, ";; flags:");
		if ((msg->flags & DNS_MESSAGEFLAG_QR) != 0)
			ADD_STRING(target, " qr");
		if ((msg->flags & DNS_MESSAGEFLAG_AA) != 0)
			ADD_STRING(target, " aa");
		if ((msg->flags & DNS_MESSAGEFLAG_TC) != 0)
			ADD_STRING(target, " tc");
		if ((msg->flags & DNS_MESSAGEFLAG_RD) != 0)
			ADD_STRING(target, " rd");
		if ((msg->flags & DNS_MESSAGEFLAG_RA) != 0)
			ADD_STRING(target, " ra");
		if ((msg->flags & DNS_MESSAGEFLAG_AD) != 0)
			ADD_STRING(target, " ad");
		if ((msg->flags & DNS_MESSAGEFLAG_CD) != 0)
			ADD_STRING(target, " cd");
		/*
		 * The final unnamed flag must be zero.
		 */
		if ((msg->flags & 0x0040U) != 0) {
			INDENT(style);
			ADD_STRING(target, "; MBZ: 0x4");
		}
		if (msg->opcode != dns_opcode_update) {
			INDENT(style);
			ADD_STRING(target, "; QUESTION: ");
		} else {
			INDENT(style);
			ADD_STRING(target, "; ZONE: ");
		}
		snprintf(buf, sizeof(buf), "%1u",
			 msg->counts[DNS_SECTION_QUESTION]);
		ADD_STRING(target, buf);
		if (msg->opcode != dns_opcode_update) {
			ADD_STRING(target, ", ANSWER: ");
		} else {
			ADD_STRING(target, ", PREREQ: ");
		}
		snprintf(buf, sizeof(buf), "%1u",
			 msg->counts[DNS_SECTION_ANSWER]);
		ADD_STRING(target, buf);
		if (msg->opcode != dns_opcode_update) {
			ADD_STRING(target, ", AUTHORITY: ");
		} else {
			ADD_STRING(target, ", UPDATE: ");
		}
		snprintf(buf, sizeof(buf), "%1u",
			msg->counts[DNS_SECTION_AUTHORITY]);
		ADD_STRING(target, buf);
		ADD_STRING(target, ", ADDITIONAL: ");
		snprintf(buf, sizeof(buf), "%1u",
			msg->counts[DNS_SECTION_ADDITIONAL]);
		ADD_STRING(target, buf);
		ADD_STRING(target, "\n");
	}
	result = dns_message_pseudosectiontotext(msg,
						 DNS_PSEUDOSECTION_OPT,
						 style, flags, target);
	if (result != ISC_R_SUCCESS)
		return (result);

	result = dns_message_sectiontotext(msg, DNS_SECTION_QUESTION,
					   style, flags, target);
	if (result != ISC_R_SUCCESS)
		return (result);
	result = dns_message_sectiontotext(msg, DNS_SECTION_ANSWER,
					   style, flags, target);
	if (result != ISC_R_SUCCESS)
		return (result);
	result = dns_message_sectiontotext(msg, DNS_SECTION_AUTHORITY,
					   style, flags, target);
	if (result != ISC_R_SUCCESS)
		return (result);
	result = dns_message_sectiontotext(msg, DNS_SECTION_ADDITIONAL,
					   style, flags, target);
	if (result != ISC_R_SUCCESS)
		return (result);

	result = dns_message_pseudosectiontotext(msg,
						 DNS_PSEUDOSECTION_TSIG,
						 style, flags, target);
	if (result != ISC_R_SUCCESS)
		return (result);

	result = dns_message_pseudosectiontotext(msg,
						 DNS_PSEUDOSECTION_SIG0,
						 style, flags, target);
	if (result != ISC_R_SUCCESS)
		return (result);

 cleanup:
	return (result);
}