static void __net_exit ping_v4_proc_exit_net(struct net *net)
{
	ping_proc_unregister(net, &ping_v4_seq_afinfo);
}