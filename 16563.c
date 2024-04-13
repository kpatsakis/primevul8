static int proc_single_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, proc_single_show, inode);
}