int fib6_rules_dump(struct net *net, struct notifier_block *nb)
{
	return fib_rules_dump(net, nb, AF_INET6);
}