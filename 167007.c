proto_get_first_protocol_field(const int proto_id, void **cookie)
{
	protocol_t *protocol = find_protocol_by_id(proto_id);

	if ((protocol == NULL) || (protocol->fields == NULL) || (protocol->fields->len == 0))
		return NULL;

	*cookie = GUINT_TO_POINTER(0 + 1);
	return (header_field_info *)g_ptr_array_index(protocol->fields, 0);
}