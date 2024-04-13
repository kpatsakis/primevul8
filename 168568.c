ncp_make_closed(struct inode *inode)
{
	int err;

	err = 0;
	mutex_lock(&NCP_FINFO(inode)->open_mutex);
	if (atomic_read(&NCP_FINFO(inode)->opened) == 1) {
		atomic_set(&NCP_FINFO(inode)->opened, 0);
		err = ncp_close_file(NCP_SERVER(inode), NCP_FINFO(inode)->file_handle);

		if (!err)
			ncp_vdbg("volnum=%d, dirent=%u, error=%d\n",
				 NCP_FINFO(inode)->volNumber,
				 NCP_FINFO(inode)->dirEntNum, err);
	}
	mutex_unlock(&NCP_FINFO(inode)->open_mutex);
	return err;
}