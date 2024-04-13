ncp_close_file(struct ncp_server *server, const char *file_id)
{
	int result;

	ncp_init_request(server);
	ncp_add_byte(server, 0);
	ncp_add_mem(server, file_id, 6);

	result = ncp_request(server, 66);
	ncp_unlock_server(server);
	return result;
}