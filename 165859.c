void ping_proc_exit(void)
{
	unregister_pernet_subsys(&ping_v4_net_ops);
}