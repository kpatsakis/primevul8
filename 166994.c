proto_get_next_protocol_field(const int proto_id, void **cookie)
{
	protocol_t *protocol = find_protocol_by_id(proto_id);
	guint       i        = GPOINTER_TO_UINT(*cookie) - 1;

	i++;

	if ((protocol->fields == NULL) || (i >= protocol->fields->len))
		return NULL;

	*cookie = GUINT_TO_POINTER(i + 1);
	return (header_field_info *)g_ptr_array_index(protocol->fields, i);
}