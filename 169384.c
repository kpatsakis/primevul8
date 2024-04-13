void tag_autodb_dirty(void)
{
	struct List *item;
	PgPool *pool;

	statlist_for_each(item, &pool_list) {
		pool = container_of(item, PgPool, head);
		if (pool->db->db_auto)
			tag_pool_dirty(pool);
	}
}