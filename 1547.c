nfs4_file_flush(struct file *file, fl_owner_t id)
{
	struct inode	*inode = file_inode(file);
	errseq_t since;

	dprintk("NFS: flush(%pD2)\n", file);

	nfs_inc_stats(inode, NFSIOS_VFSFLUSH);
	if ((file->f_mode & FMODE_WRITE) == 0)
		return 0;

	/*
	 * If we're holding a write delegation, then check if we're required
	 * to flush the i/o on close. If not, then just start the i/o now.
	 */
	if (!nfs4_delegation_flush_on_close(inode))
		return filemap_fdatawrite(file->f_mapping);

	/* Flush writes to the server and return any errors */
	since = filemap_sample_wb_err(file->f_mapping);
	nfs_wb_all(inode);
	return filemap_check_wb_err(file->f_mapping, since);
}