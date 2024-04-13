dns_message_sectiontotext(dns_message_t *msg, dns_section_t section,
			  const dns_master_style_t *style,
			  dns_messagetextflag_t flags,
			  isc_buffer_t *target) {
	dns_name_t *name, empty_name;
	dns_rdataset_t *rdataset;
	isc_result_t result = ISC_R_SUCCESS;
	bool seensoa = false;
	unsigned int saveindent;
	dns_masterstyle_flags_t sflags;

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE(target != NULL);
	REQUIRE(VALID_SECTION(section));

	saveindent = dns_master_indent;
	sflags = dns_master_styleflags(style);
	if (ISC_LIST_EMPTY(msg->sections[section]))
		goto cleanup;


	INDENT(style);
	if ((sflags & DNS_STYLEFLAG_YAML) != 0) {
		if (msg->opcode != dns_opcode_update) {
			ADD_STRING(target, sectiontext[section]);
		} else {
			ADD_STRING(target, updsectiontext[section]);
		}
		ADD_STRING(target, "_SECTION:\n");
	} else if ((flags & DNS_MESSAGETEXTFLAG_NOCOMMENTS) == 0) {
		ADD_STRING(target, ";; ");
		if (msg->opcode != dns_opcode_update) {
			ADD_STRING(target, sectiontext[section]);
		} else {
			ADD_STRING(target, updsectiontext[section]);
		}
		ADD_STRING(target, " SECTION:\n");
	}

	dns_name_init(&empty_name, NULL);
	result = dns_message_firstname(msg, section);
	if (result != ISC_R_SUCCESS) {
		goto cleanup;
	}
	if ((sflags & DNS_STYLEFLAG_YAML) != 0) {
		dns_master_indent++;
	}
	do {
		name = NULL;
		dns_message_currentname(msg, section, &name);
		for (rdataset = ISC_LIST_HEAD(name->list);
		     rdataset != NULL;
		     rdataset = ISC_LIST_NEXT(rdataset, link)) {
			if (section == DNS_SECTION_ANSWER &&
			    rdataset->type == dns_rdatatype_soa) {
				if ((flags & DNS_MESSAGETEXTFLAG_OMITSOA) != 0)
					continue;
				if (seensoa &&
				    (flags & DNS_MESSAGETEXTFLAG_ONESOA) != 0)
					continue;
				seensoa = true;
			}
			if (section == DNS_SECTION_QUESTION) {
				INDENT(style);
				if ((sflags & DNS_STYLEFLAG_YAML) != 0) {
					ADD_STRING(target, "- ");
				} else {
					ADD_STRING(target, ";");
				}
				result = dns_master_questiontotext(name,
								   rdataset,
								   style,
								   target);
			} else {
				result = dns_master_rdatasettotext(name,
								   rdataset,
								   style,
								   target);
			}
			if (result != ISC_R_SUCCESS)
				goto cleanup;
		}
		result = dns_message_nextname(msg, section);
	} while (result == ISC_R_SUCCESS);
	if ((sflags & DNS_STYLEFLAG_YAML) != 0) {
		dns_master_indent--;
	}
	if ((flags & DNS_MESSAGETEXTFLAG_NOHEADERS) == 0 &&
	    (flags & DNS_MESSAGETEXTFLAG_NOCOMMENTS) == 0 &&
	    (sflags & DNS_STYLEFLAG_YAML) == 0)
	{
		INDENT(style);
		ADD_STRING(target, "\n");
	}
	if (result == ISC_R_NOMORE)
		result = ISC_R_SUCCESS;

 cleanup:
	dns_master_indent = saveindent;
	return (result);
}