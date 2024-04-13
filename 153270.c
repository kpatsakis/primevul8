dns_message_logpacket2(dns_message_t *message,
		       const char *description, isc_sockaddr_t *address,
		       isc_logcategory_t *category, isc_logmodule_t *module,
		       int level, isc_mem_t *mctx)
{
	REQUIRE(address != NULL);

	logfmtpacket(message, description, address, category, module,
		     &dns_master_style_debug, level, mctx);
}