ncp_ClearPhysicalRecord(struct ncp_server *server, const char *file_id,
	  __u32 offset, __u32 length)
{
	int result;

	ncp_init_request(server);
	ncp_add_byte(server, 0);	/* who knows... lanalyzer says that */
	ncp_add_mem(server, file_id, 6);
	ncp_add_be32(server, offset);
	ncp_add_be32(server, length);

	if ((result = ncp_request(server, 0x1E)) != 0)
	{
		ncp_unlock_server(server);
		return result;
	}
	ncp_unlock_server(server);
	return 0;
}