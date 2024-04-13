logfmtpacket(dns_message_t *message, const char *description,
	     isc_sockaddr_t *address, isc_logcategory_t *category,
	     isc_logmodule_t *module, const dns_master_style_t *style,
	     int level, isc_mem_t *mctx)
{
	char addrbuf[ISC_SOCKADDR_FORMATSIZE] = { 0 };
	const char *newline = "\n";
	const char *space = " ";
	isc_buffer_t buffer;
	char *buf = NULL;
	int len = 1024;
	isc_result_t result;

	if (! isc_log_wouldlog(dns_lctx, level))
		return;

	/*
	 * Note that these are multiline debug messages.  We want a newline
	 * to appear in the log after each message.
	 */

	if (address != NULL)
		isc_sockaddr_format(address, addrbuf, sizeof(addrbuf));
	else
		newline = space = "";

	do {
		buf = isc_mem_get(mctx, len);
		if (buf == NULL)
			break;
		isc_buffer_init(&buffer, buf, len);
		result = dns_message_totext(message, style, 0, &buffer);
		if (result == ISC_R_NOSPACE) {
			isc_mem_put(mctx, buf, len);
			len += 1024;
		} else if (result == ISC_R_SUCCESS)
			isc_log_write(dns_lctx, category, module, level,
				      "%s%s%s%s%.*s", description, space,
				      addrbuf, newline,
				      (int)isc_buffer_usedlength(&buffer),
				      buf);
	} while (result == ISC_R_NOSPACE);

	if (buf != NULL)
		isc_mem_put(mctx, buf, len);
}