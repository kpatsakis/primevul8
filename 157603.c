static int fib6_rule_action_alt(struct fib_rule *rule, struct flowi *flp,
				int flags, struct fib_lookup_arg *arg)
{
	struct fib6_result *res = arg->result;
	struct flowi6 *flp6 = &flp->u.ip6;
	struct net *net = rule->fr_net;
	struct fib6_table *table;
	int err, *oif;
	u32 tb_id;

	switch (rule->action) {
	case FR_ACT_TO_TBL:
		break;
	case FR_ACT_UNREACHABLE:
		return -ENETUNREACH;
	case FR_ACT_PROHIBIT:
		return -EACCES;
	case FR_ACT_BLACKHOLE:
	default:
		return -EINVAL;
	}

	tb_id = fib_rule_get_table(rule, arg);
	table = fib6_get_table(net, tb_id);
	if (!table)
		return -EAGAIN;

	oif = (int *)arg->lookup_data;
	err = fib6_table_lookup(net, table, *oif, flp6, res, flags);
	if (!err && res->f6i != net->ipv6.fib6_null_entry)
		err = fib6_rule_saddr(net, rule, flags, flp6,
				      res->nh->fib_nh_dev);
	else
		err = -EAGAIN;

	return err;
}