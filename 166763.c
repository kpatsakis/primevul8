proto_get_protocol_long_name(const protocol_t *protocol)
{
	if (protocol == NULL)
		return "(none)";
	return protocol->name;
}