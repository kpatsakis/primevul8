proto_get_next_protocol(void **cookie)
{
	GList      *list_item = (GList *)*cookie;
	protocol_t *protocol;

	list_item = g_list_next(list_item);
	if (list_item == NULL)
		return -1;
	*cookie = list_item;
	protocol = (protocol_t *)list_item->data;
	return protocol->proto_id;
}