dns_message_resetsig(dns_message_t *msg) {
	REQUIRE(DNS_MESSAGE_VALID(msg));
	msg->verified_sig = 0;
	msg->verify_attempted = 0;
	msg->tsigstatus = dns_rcode_noerror;
	msg->sig0status = dns_rcode_noerror;
	msg->timeadjust = 0;
	if (msg->tsigkey != NULL) {
		dns_tsigkey_detach(&msg->tsigkey);
		msg->tsigkey = NULL;
	}
}