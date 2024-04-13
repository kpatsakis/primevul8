ncp_reply_le16(struct ncp_server *server, int offset)
{
	return get_unaligned_le16(ncp_reply_data(server, offset));
}