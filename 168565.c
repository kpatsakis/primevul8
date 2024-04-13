ncp_write_kernel(struct ncp_server *server, const char *file_id,
		 __u32 offset, __u16 to_write,
		 const char *source, int *bytes_written)
{
	int result;

	ncp_init_request(server);
	ncp_add_byte(server, 0);
	ncp_add_mem(server, file_id, 6);
	ncp_add_be32(server, offset);
	ncp_add_be16(server, to_write);
	ncp_add_mem(server, source, to_write);
	
	if ((result = ncp_request(server, 73)) == 0)
		*bytes_written = to_write;
	ncp_unlock_server(server);
	return result;
}