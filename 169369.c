static bool reuse_on_release(PgSocket *server)
{
	bool res = true;
	PgPool *pool = server->pool;
	PgSocket *client = first_socket(&pool->waiting_client_list);
	if (client) {
		activate_client(client);

		/*
		 * As the activate_client() does full read loop,
		 * then it may happen that linked client close
		 * couses server close.  Report it.
		 */
		if (server->state == SV_FREE || server->state == SV_JUSTFREE)
			res = false;
	}
	return res;
}