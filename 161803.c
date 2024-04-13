static int timer_list_open(struct inode *inode, struct file *filp)
{
	return seq_open_private(filp, &timer_list_sops,
			sizeof(struct timer_list_iter));
}