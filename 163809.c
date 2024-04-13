static int ppp_open(struct inode *inode, struct file *file)
{
	cycle_kernel_lock();
	/*
	 * This could (should?) be enforced by the permissions on /dev/ppp.
	 */
	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	return 0;
}