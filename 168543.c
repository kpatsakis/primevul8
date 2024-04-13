ncp_ObtainSpecificDirBase(struct ncp_server *server,
		__u8 nsSrc, __u8 nsDst, __u8 vol_num, __le32 dir_base,
		const char *path, /* At most 1 component */
		__le32 *dirEntNum, __le32 *DosDirNum)
{
	int result;

	ncp_init_request(server);
	ncp_add_byte(server, 6); /* subfunction */
	ncp_add_byte(server, nsSrc);
	ncp_add_byte(server, nsDst);
	ncp_add_word(server, cpu_to_le16(0x8006)); /* get all */
	ncp_add_dword(server, RIM_ALL);
	ncp_add_handle_path(server, vol_num, dir_base, 1, path);

	if ((result = ncp_request(server, 87)) != 0)
	{
		ncp_unlock_server(server);
		return result;
	}

	if (dirEntNum)
		*dirEntNum = ncp_reply_dword(server, 0x30);
	if (DosDirNum)
		*DosDirNum = ncp_reply_dword(server, 0x34);
	ncp_unlock_server(server);
	return 0;
}