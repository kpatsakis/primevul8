proto_is_pino(const protocol_t *protocol)
{
	return (protocol->parent_proto_id != -1);
}