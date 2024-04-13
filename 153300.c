dns_message_logpacket(dns_message_t *message, const char *description,
		      isc_logcategory_t *category, isc_logmodule_t *module,
		      int level, isc_mem_t *mctx)
{
	logfmtpacket(message, description, NULL, category, module,
		     &dns_master_style_debug, level, mctx);
}