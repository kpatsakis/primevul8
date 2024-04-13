static unsigned int io_file_get_flags(struct file *file)
{
	umode_t mode = file_inode(file)->i_mode;
	unsigned int res = 0;

	if (S_ISREG(mode))
		res |= FFS_ISREG;
	if (__io_file_supports_nowait(file, mode))
		res |= FFS_NOWAIT;
	return res;
}