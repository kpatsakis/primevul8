int mnt_want_write_file(struct file *file)
{
	int ret;

	sb_start_write(file_inode(file)->i_sb);
	ret = __mnt_want_write_file(file);
	if (ret)
		sb_end_write(file_inode(file)->i_sb);
	return ret;
}