proto_disable_by_default(const int proto_id)
{
	protocol_t *protocol;

	protocol = find_protocol_by_id(proto_id);
	DISSECTOR_ASSERT(protocol->can_toggle);
	DISSECTOR_ASSERT(proto_is_pino(protocol) == FALSE);
	protocol->is_enabled = FALSE;
	protocol->enabled_by_default = FALSE;
}