void init_objects(void)
{
	aatree_init(&user_tree, user_node_cmp, NULL);
	user_cache = slab_create("user_cache", sizeof(PgUser), 0, NULL, USUAL_ALLOC);
	db_cache = slab_create("db_cache", sizeof(PgDatabase), 0, NULL, USUAL_ALLOC);
	pool_cache = slab_create("pool_cache", sizeof(PgPool), 0, NULL, USUAL_ALLOC);

	if (!user_cache || !db_cache || !pool_cache)
		fatal("cannot create initial caches");
}