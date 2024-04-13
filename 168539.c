ncp_reply_data(struct ncp_server *server, int offset)
{
	return &(server->packet[sizeof(struct ncp_reply_header) + offset]);
}