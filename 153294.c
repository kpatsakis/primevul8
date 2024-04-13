dns_message_logfmtpacket(dns_message_t *message, const char *description,
			 isc_logcategory_t *category, isc_logmodule_t *module,
			 const dns_master_style_t *style, int level,
			 isc_mem_t *mctx)
{
	logfmtpacket(message, description, NULL, category, module, style,
		     level, mctx);
}