static PgPool *new_pool(PgDatabase *db, PgUser *user)
{
	PgPool *pool;

	pool = slab_alloc(pool_cache);
	if (!pool)
		return NULL;

	list_init(&pool->head);
	list_init(&pool->map_head);

	pool->user = user;
	pool->db = db;

	statlist_init(&pool->active_client_list, "active_client_list");
	statlist_init(&pool->waiting_client_list, "waiting_client_list");
	statlist_init(&pool->active_server_list, "active_server_list");
	statlist_init(&pool->idle_server_list, "idle_server_list");
	statlist_init(&pool->tested_server_list, "tested_server_list");
	statlist_init(&pool->used_server_list, "used_server_list");
	statlist_init(&pool->new_server_list, "new_server_list");
	statlist_init(&pool->cancel_req_list, "cancel_req_list");

	list_append(&user->pool_list, &pool->map_head);

	/* keep pools in db/user order to make stats faster */
	put_in_order(&pool->head, &pool_list, cmp_pool);

	return pool;
}