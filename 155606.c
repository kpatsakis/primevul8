get_clienttask(ns_clientmgr_t *manager, isc_task_t **taskp) {
	MTRACE("clienttask");

	int tid = isc_nm_tid();
	if (tid < 0) {
		tid = isc_random_uniform(manager->ncpus);
	}

	int rand = isc_random_uniform(CLIENT_NTASKS_PERCPU);
	int nexttask = (rand * manager->ncpus) + tid;
	isc_task_attach(manager->taskpool[nexttask], taskp);
}