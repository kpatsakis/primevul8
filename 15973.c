int __mnt_want_write_file(struct file *file)
{
	if (file->f_mode & FMODE_WRITER) {
		/*
		 * Superblock may have become readonly while there are still
		 * writable fd's, e.g. due to a fs error with errors=remount-ro
		 */
		if (__mnt_is_readonly(file->f_path.mnt))
			return -EROFS;
		return 0;
	}
	return __mnt_want_write(file->f_path.mnt);
}