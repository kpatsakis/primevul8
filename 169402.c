void for_each_server(PgPool *pool, void (*func)(PgSocket *sk))
{
	struct List *item;

	statlist_for_each(item, &pool->idle_server_list)
		func(container_of(item, PgSocket, head));

	statlist_for_each(item, &pool->used_server_list)
		func(container_of(item, PgSocket, head));

	statlist_for_each(item, &pool->tested_server_list)
		func(container_of(item, PgSocket, head));

	statlist_for_each(item, &pool->active_server_list)
		func(container_of(item, PgSocket, head));

	statlist_for_each(item, &pool->new_server_list)
		func(container_of(item, PgSocket, head));
}