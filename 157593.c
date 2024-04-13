static int fib6_rule_delete(struct fib_rule *rule)
{
	struct net *net = rule->fr_net;

	if (net->ipv6.fib6_rules_require_fldissect &&
	    fib_rule_requires_fldissect(rule))
		net->ipv6.fib6_rules_require_fldissect--;

	return 0;
}