ncp_LogPhysicalRecord(struct ncp_server *server, const char *file_id,
	  __u8 locktype, __u32 offset, __u32 length, __u16 timeout)
{
	int result;

	ncp_init_request(server);
	ncp_add_byte(server, locktype);
	ncp_add_mem(server, file_id, 6);
	ncp_add_be32(server, offset);
	ncp_add_be32(server, length);
	ncp_add_be16(server, timeout);

	if ((result = ncp_request(server, 0x1A)) != 0)
	{
		ncp_unlock_server(server);
		return result;
	}
	ncp_unlock_server(server);
	return 0;
}