static int __net_init fib6_rules_net_init(struct net *net)
{
	struct fib_rules_ops *ops;
	int err = -ENOMEM;

	ops = fib_rules_register(&fib6_rules_ops_template, net);
	if (IS_ERR(ops))
		return PTR_ERR(ops);

	err = fib_default_rule_add(ops, 0, RT6_TABLE_LOCAL, 0);
	if (err)
		goto out_fib6_rules_ops;

	err = fib_default_rule_add(ops, 0x7FFE, RT6_TABLE_MAIN, 0);
	if (err)
		goto out_fib6_rules_ops;

	net->ipv6.fib6_rules_ops = ops;
	net->ipv6.fib6_rules_require_fldissect = 0;
out:
	return err;

out_fib6_rules_ops:
	fib_rules_unregister(ops);
	goto out;
}