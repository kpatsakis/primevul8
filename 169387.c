void change_client_state(PgSocket *client, SocketState newstate)
{
	PgPool *pool = client->pool;

	/* remove from old location */
	switch (client->state) {
	case CL_FREE:
		break;
	case CL_JUSTFREE:
		statlist_remove(&justfree_client_list, &client->head);
		break;
	case CL_LOGIN:
		statlist_remove(&login_client_list, &client->head);
		break;
	case CL_WAITING:
		statlist_remove(&pool->waiting_client_list, &client->head);
		break;
	case CL_ACTIVE:
		statlist_remove(&pool->active_client_list, &client->head);
		break;
	case CL_CANCEL:
		statlist_remove(&pool->cancel_req_list, &client->head);
		break;
	default:
		fatal("bad cur client state: %d", client->state);
	}

	client->state = newstate;

	/* put to new location */
	switch (client->state) {
	case CL_FREE:
		varcache_clean(&client->vars);
		slab_free(client_cache, client);
		break;
	case CL_JUSTFREE:
		statlist_append(&justfree_client_list, &client->head);
		break;
	case CL_LOGIN:
		statlist_append(&login_client_list, &client->head);
		break;
	case CL_WAITING:
		statlist_append(&pool->waiting_client_list, &client->head);
		break;
	case CL_ACTIVE:
		statlist_append(&pool->active_client_list, &client->head);
		break;
	case CL_CANCEL:
		statlist_append(&pool->cancel_req_list, &client->head);
		break;
	default:
		fatal("bad new client state: %d", client->state);
	}
}