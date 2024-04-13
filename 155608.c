ns_clientmgr_destroy(ns_clientmgr_t **managerp) {
	isc_result_t result;
	ns_clientmgr_t *manager;
	bool unlock = false;

	REQUIRE(managerp != NULL);
	manager = *managerp;
	*managerp = NULL;
	REQUIRE(VALID_MANAGER(manager));

	MTRACE("destroy");

	/*
	 * Check for success because we may already be task-exclusive
	 * at this point.  Only if we succeed at obtaining an exclusive
	 * lock now will we need to relinquish it later.
	 */
	result = isc_task_beginexclusive(manager->excl);
	if (result == ISC_R_SUCCESS) {
		unlock = true;
	}

	manager->exiting = true;

	if (unlock) {
		isc_task_endexclusive(manager->excl);
	}

	if (isc_refcount_decrement(&manager->references) == 1) {
		clientmgr_destroy(manager);
	}
}