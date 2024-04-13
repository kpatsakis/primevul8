static void for_each_server_filtered(PgPool *pool, void (*func)(PgSocket *sk), bool (*filter)(PgSocket *sk, void *arg), void *filter_arg)
{
	struct List *item;
	PgSocket *sk;

	statlist_for_each(item, &pool->idle_server_list) {
		sk = container_of(item, PgSocket, head);
		if (filter(sk, filter_arg))
			func(sk);
	}

	statlist_for_each(item, &pool->used_server_list) {
		sk = container_of(item, PgSocket, head);
		if (filter(sk, filter_arg))
			func(sk);
	}

	statlist_for_each(item, &pool->tested_server_list) {
		sk = container_of(item, PgSocket, head);
		if (filter(sk, filter_arg))
			func(sk);
	}

	statlist_for_each(item, &pool->active_server_list) {
		sk = container_of(item, PgSocket, head);
		if (filter(sk, filter_arg))
			func(sk);
	}

	statlist_for_each(item, &pool->new_server_list) {
		sk = container_of(item, PgSocket, head);
		if (filter(sk, filter_arg))
			func(sk);
	}
}