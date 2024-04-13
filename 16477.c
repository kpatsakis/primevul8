static int comm_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, comm_show, inode);
}