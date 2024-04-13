int ping_proc_register(struct net *net, struct ping_seq_afinfo *afinfo)
{
	struct proc_dir_entry *p;
	p = proc_create_data(afinfo->name, S_IRUGO, net->proc_net,
			     afinfo->seq_fops, afinfo);
	if (!p)
		return -ENOMEM;
	return 0;
}