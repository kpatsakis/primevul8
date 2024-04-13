static void construct_client(void *obj)
{
	PgSocket *client = obj;

	memset(client, 0, sizeof(PgSocket));
	list_init(&client->head);
	sbuf_init(&client->sbuf, client_proto);
	client->state = CL_FREE;
}