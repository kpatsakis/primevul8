proto_get_data_protocol(void *cookie)
{
	GList *list_item = (GList *)cookie;

	protocol_t *protocol = (protocol_t *)list_item->data;
	return protocol->proto_id;
}