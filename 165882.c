int __init ping_proc_init(void)
{
	return register_pernet_subsys(&ping_v4_net_ops);
}