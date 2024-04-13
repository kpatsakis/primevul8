ns_client_recursing(ns_client_t *client) {
	REQUIRE(NS_CLIENT_VALID(client));
	REQUIRE(client->state == NS_CLIENTSTATE_WORKING);

	LOCK(&client->manager->reclock);
	client->state = NS_CLIENTSTATE_RECURSING;
	ISC_LIST_APPEND(client->manager->recursing, client, rlink);
	UNLOCK(&client->manager->reclock);
}