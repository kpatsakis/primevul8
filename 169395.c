void change_server_state(PgSocket *server, SocketState newstate)
{
	PgPool *pool = server->pool;

	/* remove from old location */
	switch (server->state) {
	case SV_FREE:
		break;
	case SV_JUSTFREE:
		statlist_remove(&justfree_server_list, &server->head);
		break;
	case SV_LOGIN:
		statlist_remove(&pool->new_server_list, &server->head);
		break;
	case SV_USED:
		statlist_remove(&pool->used_server_list, &server->head);
		break;
	case SV_TESTED:
		statlist_remove(&pool->tested_server_list, &server->head);
		break;
	case SV_IDLE:
		statlist_remove(&pool->idle_server_list, &server->head);
		break;
	case SV_ACTIVE:
		statlist_remove(&pool->active_server_list, &server->head);
		break;
	default:
		fatal("change_server_state: bad old server state: %d", server->state);
	}

	server->state = newstate;

	/* put to new location */
	switch (server->state) {
	case SV_FREE:
		varcache_clean(&server->vars);
		slab_free(server_cache, server);
		break;
	case SV_JUSTFREE:
		statlist_append(&justfree_server_list, &server->head);
		break;
	case SV_LOGIN:
		statlist_append(&pool->new_server_list, &server->head);
		break;
	case SV_USED:
		/* use LIFO */
		statlist_prepend(&pool->used_server_list, &server->head);
		break;
	case SV_TESTED:
		statlist_append(&pool->tested_server_list, &server->head);
		break;
	case SV_IDLE:
		if (server->close_needed || cf_server_round_robin)
			/* try to avoid immediate usage then */
			statlist_append(&pool->idle_server_list, &server->head);
		else
			/* otherwise use LIFO */
			statlist_prepend(&pool->idle_server_list, &server->head);
		break;
	case SV_ACTIVE:
		statlist_append(&pool->active_server_list, &server->head);
		break;
	default:
		fatal("bad server state");
	}
}