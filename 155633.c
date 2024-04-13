clientmgr_destroy(ns_clientmgr_t *manager) {
	int i;

	MTRACE("clientmgr_destroy");

	isc_refcount_destroy(&manager->references);
	manager->magic = 0;

	for (i = 0; i < manager->ncpus * CLIENT_NMCTXS_PERCPU; i++) {
		isc_mem_detach(&manager->mctxpool[i]);
	}
	isc_mem_put(manager->mctx, manager->mctxpool,
		    manager->ncpus * CLIENT_NMCTXS_PERCPU *
			    sizeof(isc_mem_t *));

	if (manager->interface != NULL) {
		ns_interface_detach(&manager->interface);
	}

	isc_mutex_destroy(&manager->lock);
	isc_mutex_destroy(&manager->reclock);

	if (manager->excl != NULL) {
		isc_task_detach(&manager->excl);
	}

	for (i = 0; i < manager->ncpus * CLIENT_NTASKS_PERCPU; i++) {
		if (manager->taskpool[i] != NULL) {
			isc_task_detach(&manager->taskpool[i]);
		}
	}
	isc_mem_put(manager->mctx, manager->taskpool,
		    manager->ncpus * CLIENT_NTASKS_PERCPU *
			    sizeof(isc_task_t *));
	ns_server_detach(&manager->sctx);

	isc_mem_put(manager->mctx, manager, sizeof(*manager));
}