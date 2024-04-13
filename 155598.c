ns_client_dumpmessage(ns_client_t *client, const char *reason) {
	isc_buffer_t buffer;
	char *buf = NULL;
	int len = 1024;
	isc_result_t result;

	if (!isc_log_wouldlog(ns_lctx, ISC_LOG_DEBUG(1))) {
		return;
	}

	/*
	 * Note that these are multiline debug messages.  We want a newline
	 * to appear in the log after each message.
	 */

	do {
		buf = isc_mem_get(client->mctx, len);
		isc_buffer_init(&buffer, buf, len);
		result = dns_message_totext(
			client->message, &dns_master_style_debug, 0, &buffer);
		if (result == ISC_R_NOSPACE) {
			isc_mem_put(client->mctx, buf, len);
			len += 1024;
		} else if (result == ISC_R_SUCCESS) {
			ns_client_log(client, NS_LOGCATEGORY_CLIENT,
				      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(1),
				      "%s\n%.*s", reason,
				      (int)isc_buffer_usedlength(&buffer), buf);
		}
	} while (result == ISC_R_NOSPACE);

	if (buf != NULL) {
		isc_mem_put(client->mctx, buf, len);
	}
}