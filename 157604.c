void fib6_rules_cleanup(void)
{
	unregister_pernet_subsys(&fib6_rules_net_ops);
}