ncp_reply_dword(struct ncp_server *server, int offset)
{
	return get_unaligned((__le32 *)ncp_reply_data(server, offset));
}