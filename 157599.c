int __init fib6_rules_init(void)
{
	return register_pernet_subsys(&fib6_rules_net_ops);
}