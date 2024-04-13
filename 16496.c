static int lstats_open(struct inode *inode, struct file *file)
{
	return single_open(file, lstats_show_proc, inode);
}