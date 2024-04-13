static void construct_server(void *obj)
{
	PgSocket *server = obj;

	memset(server, 0, sizeof(PgSocket));
	list_init(&server->head);
	sbuf_init(&server->sbuf, server_proto);
	server->state = SV_FREE;
}