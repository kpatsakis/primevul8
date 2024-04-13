void tag_database_dirty(PgDatabase *db)
{
	struct List *item;
	PgPool *pool;

	statlist_for_each(item, &pool_list) {
		pool = container_of(item, PgPool, head);
		if (pool->db == db)
			tag_pool_dirty(pool);
	}
}