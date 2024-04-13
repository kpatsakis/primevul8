maybe_clear_ad(dns_message_t *msg, dns_section_t sectionid) {
	if (msg->counts[sectionid] == 0 &&
	    (sectionid == DNS_SECTION_ANSWER ||
	     (sectionid == DNS_SECTION_AUTHORITY &&
	      msg->counts[DNS_SECTION_ANSWER] == 0)))
		msg->flags &= ~DNS_MESSAGEFLAG_AD;
}