proto_set_cant_toggle(const int proto_id)
{
	protocol_t *protocol;

	protocol = find_protocol_by_id(proto_id);
	protocol->can_toggle = FALSE;
}