static int ping_seq_open(struct inode *inode, struct file *file)
{
	struct ping_seq_afinfo *afinfo = PDE_DATA(inode);
	return seq_open_net(inode, file, &afinfo->seq_ops,
			   sizeof(struct ping_iter_state));
}