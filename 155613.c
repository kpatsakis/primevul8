ns__client_put_cb(void *client0) {
	ns_client_t *client = client0;

	ns_client_log(client, DNS_LOGCATEGORY_SECURITY, NS_LOGMODULE_CLIENT,
		      ISC_LOG_DEBUG(3), "freeing client");

	/*
	 * Call this first because it requires a valid client.
	 */
	ns_query_free(client);

	client->magic = 0;
	client->shuttingdown = true;

	if (client->manager != NULL) {
		clientmgr_detach(&client->manager);
	}

	isc_mem_put(client->mctx, client->sendbuf, NS_CLIENT_SEND_BUFFER_SIZE);
	if (client->opt != NULL) {
		INSIST(dns_rdataset_isassociated(client->opt));
		dns_rdataset_disassociate(client->opt);
		dns_message_puttemprdataset(client->message, &client->opt);
	}

	dns_message_detach(&client->message);

	/*
	 * Detaching the task must be done after unlinking from
	 * the manager's lists because the manager accesses
	 * client->task.
	 */
	if (client->task != NULL) {
		isc_task_detach(&client->task);
	}

	/*
	 * Destroy the fetchlock mutex that was created in
	 * ns_query_init().
	 */
	isc_mutex_destroy(&client->query.fetchlock);

	if (client->sctx != NULL) {
		ns_server_detach(&client->sctx);
	}

	if (client->mctx != NULL) {
		isc_mem_detach(&client->mctx);
	}
}