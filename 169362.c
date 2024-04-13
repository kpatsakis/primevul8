void init_caches(void)
{
	server_cache = slab_create("server_cache", sizeof(PgSocket), 0, construct_server, USUAL_ALLOC);
	client_cache = slab_create("client_cache", sizeof(PgSocket), 0, construct_client, USUAL_ALLOC);
	iobuf_cache = slab_create("iobuf_cache", IOBUF_SIZE, 0, do_iobuf_reset, USUAL_ALLOC);
}