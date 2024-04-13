void ping_proc_unregister(struct net *net, struct ping_seq_afinfo *afinfo)
{
	remove_proc_entry(afinfo->name, net->proc_net);
}