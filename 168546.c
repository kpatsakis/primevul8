ncp_reply_be16(struct ncp_server *server, int offset)
{
	return get_unaligned_be16(ncp_reply_data(server, offset));
}