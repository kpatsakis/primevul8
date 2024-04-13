ncp_negotiate_buffersize(struct ncp_server *server, int size, int *target)
{
	int result;

	ncp_init_request(server);
	ncp_add_be16(server, size);

	if ((result = ncp_request(server, 33)) != 0) {
		ncp_unlock_server(server);
		return result;
	}
	*target = min_t(unsigned int, ncp_reply_be16(server, 0), size);

	ncp_unlock_server(server);
	return 0;
}