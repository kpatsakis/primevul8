int get_active_server_count(void)
{
	return slab_active_count(server_cache);
}