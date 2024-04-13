ncp_del_file_or_subdir2(struct ncp_server *server,
			struct dentry *dentry)
{
	struct inode *inode = d_inode(dentry);
	__u8  volnum;
	__le32 dirent;

	if (!inode) {
		return 0xFF;	/* Any error */
	}
	volnum = NCP_FINFO(inode)->volNumber;
	dirent = NCP_FINFO(inode)->DosDirNum;
	return ncp_DeleteNSEntry(server, 1, volnum, dirent, NULL, NW_NS_DOS, cpu_to_le16(0x8006));
}