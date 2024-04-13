get_clientmctx(ns_clientmgr_t *manager, isc_mem_t **mctxp) {
	isc_mem_t *clientmctx;
	MTRACE("clientmctx");

	int tid = isc_nm_tid();
	if (tid < 0) {
		tid = isc_random_uniform(manager->ncpus);
	}
	int rand = isc_random_uniform(CLIENT_NMCTXS_PERCPU);
	int nextmctx = (rand * manager->ncpus) + tid;
	clientmctx = manager->mctxpool[nextmctx];

	isc_mem_attach(clientmctx, mctxp);
}