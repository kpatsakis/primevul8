ns_client_endrequest(ns_client_t *client) {
	INSIST(client->nupdates == 0);
	INSIST(client->state == NS_CLIENTSTATE_WORKING ||
	       client->state == NS_CLIENTSTATE_RECURSING);

	CTRACE("endrequest");

	if (client->state == NS_CLIENTSTATE_RECURSING) {
		LOCK(&client->manager->reclock);
		if (ISC_LINK_LINKED(client, rlink)) {
			ISC_LIST_UNLINK(client->manager->recursing, client,
					rlink);
		}
		UNLOCK(&client->manager->reclock);
	}

	if (client->cleanup != NULL) {
		(client->cleanup)(client);
		client->cleanup = NULL;
	}

	if (client->view != NULL) {
#ifdef ENABLE_AFL
		if (client->sctx->fuzztype == isc_fuzz_resolver) {
			dns_cache_clean(client->view->cache, INT_MAX);
			dns_adb_flush(client->view->adb);
		}
#endif /* ifdef ENABLE_AFL */
		dns_view_detach(&client->view);
	}
	if (client->opt != NULL) {
		INSIST(dns_rdataset_isassociated(client->opt));
		dns_rdataset_disassociate(client->opt);
		dns_message_puttemprdataset(client->message, &client->opt);
	}

	client->signer = NULL;
	client->udpsize = 512;
	client->extflags = 0;
	client->ednsversion = -1;
	dns_ecs_init(&client->ecs);
	dns_message_reset(client->message, DNS_MESSAGE_INTENTPARSE);

	/*
	 * Clean up from recursion - normally this would be done in
	 * fetch_callback(), but if we're shutting down and canceling then
	 * it might not have happened.
	 */
	if (client->recursionquota != NULL) {
		isc_quota_detach(&client->recursionquota);
		if (client->query.prefetch == NULL) {
			ns_stats_decrement(client->sctx->nsstats,
					   ns_statscounter_recursclients);
		}
	}

	/*
	 * Clear all client attributes that are specific to the request
	 */
	client->attributes = 0;
#ifdef ENABLE_AFL
	if (client->sctx->fuzznotify != NULL &&
	    (client->sctx->fuzztype == isc_fuzz_client ||
	     client->sctx->fuzztype == isc_fuzz_tcpclient ||
	     client->sctx->fuzztype == isc_fuzz_resolver))
	{
		client->sctx->fuzznotify();
	}
#endif /* ENABLE_AFL */
}