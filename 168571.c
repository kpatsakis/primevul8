ncp_del_file_or_subdir(struct ncp_server *server,
		       struct inode *dir, const char *name)
{
	__u8  volnum = NCP_FINFO(dir)->volNumber;
	__le32 dirent = NCP_FINFO(dir)->dirEntNum;
	int name_space;

	name_space = server->name_space[volnum];
#ifdef CONFIG_NCPFS_NFS_NS
	if (name_space == NW_NS_NFS)
 	{
 		int result;
 
		result=ncp_obtain_DOS_dir_base(server, name_space, volnum, dirent, name, &dirent);
 		if (result) return result;
		name = NULL;
		name_space = NW_NS_DOS;
 	}
#endif	/* CONFIG_NCPFS_NFS_NS */
	return ncp_DeleteNSEntry(server, 1, volnum, dirent, name, name_space, cpu_to_le16(0x8006));
}