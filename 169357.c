int get_active_client_count(void)
{
	return slab_active_count(client_cache);
}