static int fib6_rule_configure(struct fib_rule *rule, struct sk_buff *skb,
			       struct fib_rule_hdr *frh,
			       struct nlattr **tb,
			       struct netlink_ext_ack *extack)
{
	int err = -EINVAL;
	struct net *net = sock_net(skb->sk);
	struct fib6_rule *rule6 = (struct fib6_rule *) rule;

	if (rule->action == FR_ACT_TO_TBL && !rule->l3mdev) {
		if (rule->table == RT6_TABLE_UNSPEC) {
			NL_SET_ERR_MSG(extack, "Invalid table");
			goto errout;
		}

		if (fib6_new_table(net, rule->table) == NULL) {
			err = -ENOBUFS;
			goto errout;
		}
	}

	if (frh->src_len)
		rule6->src.addr = nla_get_in6_addr(tb[FRA_SRC]);

	if (frh->dst_len)
		rule6->dst.addr = nla_get_in6_addr(tb[FRA_DST]);

	rule6->src.plen = frh->src_len;
	rule6->dst.plen = frh->dst_len;
	rule6->tclass = frh->tos;

	if (fib_rule_requires_fldissect(rule))
		net->ipv6.fib6_rules_require_fldissect++;

	net->ipv6.fib6_has_custom_rules = true;
	err = 0;
errout:
	return err;
}