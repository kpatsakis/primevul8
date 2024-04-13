proto_get_protocol_short_name(const protocol_t *protocol)
{
	if (protocol == NULL)
		return "(none)";
	return protocol->short_name;
}