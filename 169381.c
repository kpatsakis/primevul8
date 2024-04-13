PgPool *get_pool(PgDatabase *db, PgUser *user)
{
	struct List *item;
	PgPool *pool;

	if (!db || !user)
		return NULL;

	list_for_each(item, &user->pool_list) {
		pool = container_of(item, PgPool, map_head);
		if (pool->db == db)
			return pool;
	}

	return new_pool(db, user);
}