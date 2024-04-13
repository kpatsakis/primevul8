void __init ping_init(void)
{
	int i;

	for (i = 0; i < PING_HTABLE_SIZE; i++)
		INIT_HLIST_NULLS_HEAD(&ping_table.hash[i], i);
	rwlock_init(&ping_table.lock);
}