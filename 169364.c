void tag_host_addr_dirty(const char *host, const struct sockaddr *sa)
{
	struct List *item;
	PgPool *pool;
	PgAddr addr;

	memset(&addr, 0, sizeof(addr));
	pga_copy(&addr, sa);

	statlist_for_each(item, &pool_list) {
		pool = container_of(item, PgPool, head);
		if (pool->db->host && strcmp(host, pool->db->host) == 0) {
			for_each_server_filtered(pool, tag_dirty, server_remote_addr_filter, &addr);
		}
	}
}