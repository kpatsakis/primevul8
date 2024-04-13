struct dst_entry *fib6_rule_lookup(struct net *net, struct flowi6 *fl6,
				   const struct sk_buff *skb,
				   int flags, pol_lookup_t lookup)
{
	if (net->ipv6.fib6_has_custom_rules) {
		struct fib6_result res = {};
		struct fib_lookup_arg arg = {
			.lookup_ptr = lookup,
			.lookup_data = skb,
			.result = &res,
			.flags = FIB_LOOKUP_NOREF,
		};

		/* update flow if oif or iif point to device enslaved to l3mdev */
		l3mdev_update_flow(net, flowi6_to_flowi(fl6));

		fib_rules_lookup(net->ipv6.fib6_rules_ops,
				 flowi6_to_flowi(fl6), flags, &arg);

		if (res.rt6)
			return &res.rt6->dst;
	} else {
		struct rt6_info *rt;

		rt = lookup(net, net->ipv6.fib6_local_tbl, fl6, skb, flags);
		if (rt != net->ipv6.ip6_null_entry && rt->dst.error != -EAGAIN)
			return &rt->dst;
		ip6_rt_put_flags(rt, flags);
		rt = lookup(net, net->ipv6.fib6_main_tbl, fl6, skb, flags);
		if (rt->dst.error != -EAGAIN)
			return &rt->dst;
		ip6_rt_put_flags(rt, flags);
	}

	if (!(flags & RT6_LOOKUP_F_DST_NOREF))
		dst_hold(&net->ipv6.ip6_null_entry->dst);
	return &net->ipv6.ip6_null_entry->dst;
}