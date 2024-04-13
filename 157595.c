static int fib6_rule_fill(struct fib_rule *rule, struct sk_buff *skb,
			  struct fib_rule_hdr *frh)
{
	struct fib6_rule *rule6 = (struct fib6_rule *) rule;

	frh->dst_len = rule6->dst.plen;
	frh->src_len = rule6->src.plen;
	frh->tos = rule6->tclass;

	if ((rule6->dst.plen &&
	     nla_put_in6_addr(skb, FRA_DST, &rule6->dst.addr)) ||
	    (rule6->src.plen &&
	     nla_put_in6_addr(skb, FRA_SRC, &rule6->src.addr)))
		goto nla_put_failure;
	return 0;

nla_put_failure:
	return -ENOBUFS;
}