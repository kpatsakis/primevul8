void accept_cancel_request(PgSocket *req)
{
	struct List *pitem, *citem;
	PgPool *pool = NULL;
	PgSocket *server = NULL, *client, *main_client = NULL;

	Assert(req->state == CL_LOGIN);

	/* find real client this is for */
	statlist_for_each(pitem, &pool_list) {
		pool = container_of(pitem, PgPool, head);
		statlist_for_each(citem, &pool->active_client_list) {
			client = container_of(citem, PgSocket, head);
			if (memcmp(client->cancel_key, req->cancel_key, 8) == 0) {
				main_client = client;
				goto found;
			}
		}
	}
found:

	/* wrong key */
	if (!main_client) {
		disconnect_client(req, false, "failed cancel request");
		return;
	}

	/* not linked client, just drop it then */
	if (!main_client->link) {
		bool res;

		/* let administrative cancel be handled elsewhere */
		if (main_client->pool->db->admin) {
			disconnect_client(req, false, "cancel request for console client");
			admin_handle_cancel(main_client);
			return;
		}

		disconnect_client(req, false, "cancel request for idle client");

		/* notify readiness */
		SEND_ReadyForQuery(res, main_client);
		if (!res)
			disconnect_client(main_client, true, "ReadyForQuery for main_client failed");
		return;
	}

	/* drop the connection, if fails, retry later in justfree list */
	if (!sbuf_close(&req->sbuf))
		log_noise("sbuf_close failed, retry later");

	/* remember server key */
	server = main_client->link;
	memcpy(req->cancel_key, server->cancel_key, 8);

	/* attach to target pool */
	req->pool = pool;
	change_client_state(req, CL_CANCEL);

	/* need fresh connection */
	launch_new_connection(pool);
}