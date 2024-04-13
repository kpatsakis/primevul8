proto_get_protocol_filter_name(const int proto_id)
{
	protocol_t *protocol;

	protocol = find_protocol_by_id(proto_id);
	if (protocol == NULL)
		return "(none)";
	return protocol->filter_name;
}