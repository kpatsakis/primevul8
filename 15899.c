void mnt_drop_write_file(struct file *file)
{
	__mnt_drop_write_file(file);
	sb_end_write(file_inode(file)->i_sb);
}