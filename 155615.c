ns_client_killoldestquery(ns_client_t *client) {
	ns_client_t *oldest;
	REQUIRE(NS_CLIENT_VALID(client));

	LOCK(&client->manager->reclock);
	oldest = ISC_LIST_HEAD(client->manager->recursing);
	if (oldest != NULL) {
		ISC_LIST_UNLINK(client->manager->recursing, oldest, rlink);
		ns_query_cancel(oldest);
		ns_stats_increment(client->sctx->nsstats,
				   ns_statscounter_reclimitdropped);
	}
	UNLOCK(&client->manager->reclock);
}