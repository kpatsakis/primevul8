static int fib6_rule_saddr(struct net *net, struct fib_rule *rule, int flags,
			   struct flowi6 *flp6, const struct net_device *dev)
{
	struct fib6_rule *r = (struct fib6_rule *)rule;

	/* If we need to find a source address for this traffic,
	 * we check the result if it meets requirement of the rule.
	 */
	if ((rule->flags & FIB_RULE_FIND_SADDR) &&
	    r->src.plen && !(flags & RT6_LOOKUP_F_HAS_SADDR)) {
		struct in6_addr saddr;

		if (ipv6_dev_get_saddr(net, dev, &flp6->daddr,
				       rt6_flags2srcprefs(flags), &saddr))
			return -EAGAIN;

		if (!ipv6_prefix_equal(&saddr, &r->src.addr, r->src.plen))
			return -EAGAIN;

		flp6->saddr = saddr;
	}

	return 0;
}