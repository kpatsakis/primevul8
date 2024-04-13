static void __net_exit fib6_rules_net_exit(struct net *net)
{
	rtnl_lock();
	fib_rules_unregister(net->ipv6.fib6_rules_ops);
	rtnl_unlock();
}