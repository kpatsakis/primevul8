bool check_fast_fail(PgSocket *client)
{
	int cnt;
	PgPool *pool = client->pool;

	/* reject if no servers and last connect failed */
	if (!pool->last_connect_failed)
		return true;
	cnt = pool_server_count(pool) - statlist_count(&pool->new_server_list);
	if (cnt)
		return true;
	disconnect_client(client, true, "pgbouncer cannot connect to server");

	/* usual relaunch wont work, as there are no waiting clients */
	launch_new_connection(pool);

	return false;
}