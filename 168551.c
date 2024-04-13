ncp_negotiate_size_and_options(struct ncp_server *server, 
	int size, int options, int *ret_size, int *ret_options) {
	int result;

	/* there is minimum */
	if (size < NCP_BLOCK_SIZE) size = NCP_BLOCK_SIZE;

	ncp_init_request(server);
	ncp_add_be16(server, size);
	ncp_add_byte(server, options);
	
	if ((result = ncp_request(server, 0x61)) != 0)
	{
		ncp_unlock_server(server);
		return result;
	}

	/* NCP over UDP returns 0 (!!!) */
	result = ncp_reply_be16(server, 0);
	if (result >= NCP_BLOCK_SIZE)
		size = min(result, size);
	*ret_size = size;
	*ret_options = ncp_reply_byte(server, 4);

	ncp_unlock_server(server);
	return 0;
}