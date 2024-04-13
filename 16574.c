static int sched_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, sched_show, inode);
}