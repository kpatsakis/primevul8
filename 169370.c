static bool life_over(PgSocket *server)
{
	PgPool *pool = server->pool;
	usec_t lifetime_kill_gap = 0;
	usec_t now = get_cached_time();
	usec_t age = now - server->connect_time;
	usec_t last_kill = now - pool->last_lifetime_disconnect;

	if (age < cf_server_lifetime)
		return false;

	if (pool->db->pool_size > 0)
		lifetime_kill_gap = cf_server_lifetime / pool->db->pool_size;

	if (last_kill >= lifetime_kill_gap)
		return true;

	return false;
}