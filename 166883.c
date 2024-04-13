proto_can_toggle_protocol(const int proto_id)
{
	protocol_t *protocol;

	protocol = find_protocol_by_id(proto_id);
	//parent protocol determines toggling for helper dissectors
	if (proto_is_pino(protocol))
		return proto_can_toggle_protocol(protocol->parent_proto_id);

	return protocol->can_toggle;
}