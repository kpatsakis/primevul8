ns_client_log(ns_client_t *client, isc_logcategory_t *category,
	      isc_logmodule_t *module, int level, const char *fmt, ...) {
	va_list ap;

	if (!isc_log_wouldlog(ns_lctx, level)) {
		return;
	}

	va_start(ap, fmt);
	ns_client_logv(client, category, module, level, fmt, ap);
	va_end(ap);
}