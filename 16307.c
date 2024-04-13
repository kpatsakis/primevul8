static bool __io_file_supports_nowait(struct file *file, umode_t mode)
{
	if (S_ISBLK(mode)) {
		if (IS_ENABLED(CONFIG_BLOCK) &&
		    io_bdev_nowait(I_BDEV(file->f_mapping->host)))
			return true;
		return false;
	}
	if (S_ISSOCK(mode))
		return true;
	if (S_ISREG(mode)) {
		if (IS_ENABLED(CONFIG_BLOCK) &&
		    io_bdev_nowait(file->f_inode->i_sb->s_bdev) &&
		    file->f_op != &io_uring_fops)
			return true;
		return false;
	}

	/* any ->read/write should understand O_NONBLOCK */
	if (file->f_flags & O_NONBLOCK)
		return true;
	return file->f_mode & FMODE_NOWAIT;
}