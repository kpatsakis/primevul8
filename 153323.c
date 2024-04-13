dns_message_logfmtpacket2(dns_message_t *message,
			  const char *description, isc_sockaddr_t *address,
			  isc_logcategory_t *category, isc_logmodule_t *module,
			  const dns_master_style_t *style, int level,
			  isc_mem_t *mctx)
{
	REQUIRE(address != NULL);

	logfmtpacket(message, description, address, category, module, style,
		     level, mctx);
}