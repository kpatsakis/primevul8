ncp_obtain_DOS_dir_base(struct ncp_server *server,
		__u8 ns, __u8 volnum, __le32 dirent,
		const char *path, /* At most 1 component */
		__le32 *DOS_dir_base)
{
	int result;

	ncp_init_request(server);
	ncp_add_byte(server, 6); /* subfunction */
	ncp_add_byte(server, ns);
	ncp_add_byte(server, ns);
	ncp_add_word(server, cpu_to_le16(0x8006)); /* get all */
	ncp_add_dword(server, RIM_DIRECTORY);
	ncp_add_handle_path(server, volnum, dirent, 1, path);

	if ((result = ncp_request(server, 87)) == 0)
	{
	   	if (DOS_dir_base) *DOS_dir_base=ncp_reply_dword(server, 0x34);
	}
	ncp_unlock_server(server);
	return result;
}