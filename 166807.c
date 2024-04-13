proto_reenable_all(void)
{
	protocol_t *protocol;
	GList      *list_item = protocols;

	if (protocols == NULL)
		return;

	while (list_item) {
		protocol = (protocol_t *)list_item->data;
		if (protocol->can_toggle && protocol->enabled_by_default)
			protocol->is_enabled = TRUE;
		list_item = g_list_next(list_item);
	}
}