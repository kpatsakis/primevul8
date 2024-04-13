ncp_initialize_search(struct ncp_server *server, struct inode *dir,
			struct nw_search_sequence *target)
{
	__u8  volnum = NCP_FINFO(dir)->volNumber;
	__le32 dirent = NCP_FINFO(dir)->dirEntNum;
	int result;

	ncp_init_request(server);
	ncp_add_byte(server, 2);	/* subfunction */
	ncp_add_byte(server, server->name_space[volnum]);
	ncp_add_byte(server, 0);	/* reserved */
	ncp_add_handle_path(server, volnum, dirent, 1, NULL);

	result = ncp_request(server, 87);
	if (result)
		goto out;
	memcpy(target, ncp_reply_data(server, 0), sizeof(*target));

out:
	ncp_unlock_server(server);
	return result;
}