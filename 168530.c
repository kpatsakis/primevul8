ncp_RenameNSEntry(struct ncp_server *server,
		  struct inode *old_dir, const char *old_name, __le16 old_type,
		  struct inode *new_dir, const char *new_name)
{
	int result = -EINVAL;

	if ((old_dir == NULL) || (old_name == NULL) ||
	    (new_dir == NULL) || (new_name == NULL))
		goto out;

	ncp_init_request(server);
	ncp_add_byte(server, 4);	/* subfunction */
	ncp_add_byte(server, server->name_space[NCP_FINFO(old_dir)->volNumber]);
	ncp_add_byte(server, 1);	/* rename flag */
	ncp_add_word(server, old_type);	/* search attributes */

	/* source Handle Path */
	ncp_add_byte(server, NCP_FINFO(old_dir)->volNumber);
	ncp_add_dword(server, NCP_FINFO(old_dir)->dirEntNum);
	ncp_add_byte(server, 1);
	ncp_add_byte(server, 1);	/* 1 source component */

	/* dest Handle Path */
	ncp_add_byte(server, NCP_FINFO(new_dir)->volNumber);
	ncp_add_dword(server, NCP_FINFO(new_dir)->dirEntNum);
	ncp_add_byte(server, 1);
	ncp_add_byte(server, 1);	/* 1 destination component */

	/* source path string */
	ncp_add_pstring(server, old_name);
	/* dest path string */
	ncp_add_pstring(server, new_name);

	result = ncp_request(server, 87);
	ncp_unlock_server(server);
out:
	return result;
}