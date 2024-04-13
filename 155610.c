ns_clientmgr_create(isc_mem_t *mctx, ns_server_t *sctx, isc_taskmgr_t *taskmgr,
		    isc_timermgr_t *timermgr, ns_interface_t *interface,
		    int ncpus, ns_clientmgr_t **managerp) {
	ns_clientmgr_t *manager;
	isc_result_t result;
	int i;
	int npools;

	manager = isc_mem_get(mctx, sizeof(*manager));
	*manager = (ns_clientmgr_t){ .magic = 0 };

	isc_mutex_init(&manager->lock);
	isc_mutex_init(&manager->reclock);

	manager->excl = NULL;
	result = isc_taskmgr_excltask(taskmgr, &manager->excl);
	if (result != ISC_R_SUCCESS) {
		goto cleanup_reclock;
	}

	manager->mctx = mctx;
	manager->taskmgr = taskmgr;
	manager->timermgr = timermgr;
	manager->ncpus = ncpus;

	ns_interface_attach(interface, &manager->interface);

	manager->exiting = false;
	int ntasks = CLIENT_NTASKS_PERCPU * manager->ncpus;
	manager->taskpool = isc_mem_get(mctx, ntasks * sizeof(isc_task_t *));
	for (i = 0; i < ntasks; i++) {
		manager->taskpool[i] = NULL;
		result = isc_task_create_bound(manager->taskmgr, 20,
					       &manager->taskpool[i],
					       i % CLIENT_NTASKS_PERCPU);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
	}
	isc_refcount_init(&manager->references, 1);
	manager->sctx = NULL;
	ns_server_attach(sctx, &manager->sctx);

	ISC_LIST_INIT(manager->recursing);

	npools = CLIENT_NMCTXS_PERCPU * manager->ncpus;
	manager->mctxpool = isc_mem_get(manager->mctx,
					npools * sizeof(isc_mem_t *));
	for (i = 0; i < npools; i++) {
		manager->mctxpool[i] = NULL;
		isc_mem_create(&manager->mctxpool[i]);
		isc_mem_setname(manager->mctxpool[i], "client", NULL);
	}

	manager->magic = MANAGER_MAGIC;

	MTRACE("create");

	*managerp = manager;

	return (ISC_R_SUCCESS);

cleanup_reclock:
	isc_mutex_destroy(&manager->reclock);
	isc_mutex_destroy(&manager->lock);

	isc_mem_put(mctx, manager, sizeof(*manager));

	return (result);
}