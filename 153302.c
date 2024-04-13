dns_message_reply(dns_message_t *msg, bool want_question_section) {
	unsigned int clear_from;
	isc_result_t result;

	REQUIRE(DNS_MESSAGE_VALID(msg));
	REQUIRE((msg->flags & DNS_MESSAGEFLAG_QR) == 0);

	if (!msg->header_ok)
		return (DNS_R_FORMERR);
	if (msg->opcode != dns_opcode_query &&
	    msg->opcode != dns_opcode_notify)
		want_question_section = false;
	if (msg->opcode == dns_opcode_update)
		clear_from = DNS_SECTION_PREREQUISITE;
	else if (want_question_section) {
		if (!msg->question_ok)
			return (DNS_R_FORMERR);
		clear_from = DNS_SECTION_ANSWER;
	} else
		clear_from = DNS_SECTION_QUESTION;
	msg->from_to_wire = DNS_MESSAGE_INTENTRENDER;
	msgresetnames(msg, clear_from);
	msgresetopt(msg);
	msgresetsigs(msg, true);
	msginitprivate(msg);
	/*
	 * We now clear most flags and then set QR, ensuring that the
	 * reply's flags will be in a reasonable state.
	 */
	if (msg->opcode == dns_opcode_query)
		msg->flags &= DNS_MESSAGE_REPLYPRESERVE;
	else
		msg->flags = 0;
	msg->flags |= DNS_MESSAGEFLAG_QR;

	/*
	 * This saves the query TSIG status, if the query was signed, and
	 * reserves space in the reply for the TSIG.
	 */
	if (msg->tsigkey != NULL) {
		unsigned int otherlen = 0;
		msg->querytsigstatus = msg->tsigstatus;
		msg->tsigstatus = dns_rcode_noerror;
		if (msg->querytsigstatus == dns_tsigerror_badtime)
			otherlen = 6;
		msg->sig_reserved = spacefortsig(msg->tsigkey, otherlen);
		result = dns_message_renderreserve(msg, msg->sig_reserved);
		if (result != ISC_R_SUCCESS) {
			msg->sig_reserved = 0;
			return (result);
		}
	}
	if (msg->saved.base != NULL) {
		msg->query.base = msg->saved.base;
		msg->query.length = msg->saved.length;
		msg->free_query = msg->free_saved;
		msg->saved.base = NULL;
		msg->saved.length = 0;
		msg->free_saved = 0;
	}

	return (ISC_R_SUCCESS);
}