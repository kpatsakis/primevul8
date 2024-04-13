unsigned int fib6_rules_seq_read(struct net *net)
{
	return fib_rules_seq_read(net, AF_INET6);
}