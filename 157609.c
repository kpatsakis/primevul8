static int fib6_rule_action(struct fib_rule *rule, struct flowi *flp,
			    int flags, struct fib_lookup_arg *arg)
{
	if (arg->lookup_ptr == fib6_table_lookup)
		return fib6_rule_action_alt(rule, flp, flags, arg);

	return __fib6_rule_action(rule, flp, flags, arg);
}