clientmgr_detach(ns_clientmgr_t **mp) {
	int32_t oldrefs;
	ns_clientmgr_t *mgr = *mp;
	*mp = NULL;

	oldrefs = isc_refcount_decrement(&mgr->references);
	isc_log_write(ns_lctx, NS_LOGCATEGORY_CLIENT, NS_LOGMODULE_CLIENT,
		      ISC_LOG_DEBUG(3), "clientmgr @%p detach: %d", mgr,
		      oldrefs - 1);
	if (oldrefs == 1) {
		clientmgr_destroy(mgr);
	}
}