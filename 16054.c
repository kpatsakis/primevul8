void __mnt_drop_write_file(struct file *file)
{
	if (!(file->f_mode & FMODE_WRITER))
		__mnt_drop_write(file->f_path.mnt);
}