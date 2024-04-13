proto_is_protocol_enabled_by_default(const protocol_t *protocol)
{
	//parent protocol determines enable/disable for helper dissectors
	if (proto_is_pino(protocol))
		return proto_is_protocol_enabled_by_default(find_protocol_by_id(protocol->parent_proto_id));

	return protocol->enabled_by_default;
}