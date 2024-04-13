int fib6_lookup(struct net *net, int oif, struct flowi6 *fl6,
		struct fib6_result *res, int flags)
{
	int err;

	if (net->ipv6.fib6_has_custom_rules) {
		struct fib_lookup_arg arg = {
			.lookup_ptr = fib6_table_lookup,
			.lookup_data = &oif,
			.result = res,
			.flags = FIB_LOOKUP_NOREF,
		};

		l3mdev_update_flow(net, flowi6_to_flowi(fl6));

		err = fib_rules_lookup(net->ipv6.fib6_rules_ops,
				       flowi6_to_flowi(fl6), flags, &arg);
	} else {
		err = fib6_table_lookup(net, net->ipv6.fib6_local_tbl, oif,
					fl6, res, flags);
		if (err || res->f6i == net->ipv6.fib6_null_entry)
			err = fib6_table_lookup(net, net->ipv6.fib6_main_tbl,
						oif, fl6, res, flags);
	}

	return err;
}