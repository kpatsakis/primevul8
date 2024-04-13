static bool fib6_rule_matchall(const struct fib_rule *rule)
{
	struct fib6_rule *r = container_of(rule, struct fib6_rule, common);

	if (r->dst.plen || r->src.plen || r->tclass)
		return false;
	return fib_rule_matchall(rule);
}