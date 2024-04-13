ncp_get_volume_root(struct ncp_server *server,
		    const char *volname, __u32* volume, __le32* dirent, __le32* dosdirent)
{
	int result;

	ncp_dbg(1, "looking up vol %s\n", volname);

	ncp_init_request(server);
	ncp_add_byte(server, 22);	/* Subfunction: Generate dir handle */
	ncp_add_byte(server, 0);	/* DOS namespace */
	ncp_add_byte(server, 0);	/* reserved */
	ncp_add_byte(server, 0);	/* reserved */
	ncp_add_byte(server, 0);	/* reserved */

	ncp_add_byte(server, 0);	/* faked volume number */
	ncp_add_dword(server, 0);	/* faked dir_base */
	ncp_add_byte(server, 0xff);	/* Don't have a dir_base */
	ncp_add_byte(server, 1);	/* 1 path component */
	ncp_add_pstring(server, volname);

	if ((result = ncp_request(server, 87)) != 0) {
		ncp_unlock_server(server);
		return result;
	}
	*dirent = *dosdirent = ncp_reply_dword(server, 4);
	*volume = ncp_reply_byte(server, 8);
	ncp_unlock_server(server);
	return 0;
}