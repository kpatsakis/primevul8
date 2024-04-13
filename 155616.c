clientmgr_attach(ns_clientmgr_t *source, ns_clientmgr_t **targetp) {
	int32_t oldrefs;

	REQUIRE(VALID_MANAGER(source));
	REQUIRE(targetp != NULL && *targetp == NULL);

	oldrefs = isc_refcount_increment0(&source->references);
	isc_log_write(ns_lctx, NS_LOGCATEGORY_CLIENT, NS_LOGMODULE_CLIENT,
		      ISC_LOG_DEBUG(3), "clientmgr @%p attach: %d", source,
		      oldrefs + 1);

	*targetp = source;
}