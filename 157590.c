bool fib6_rule_default(const struct fib_rule *rule)
{
	if (!fib6_rule_matchall(rule) || rule->action != FR_ACT_TO_TBL ||
	    rule->l3mdev)
		return false;
	if (rule->table != RT6_TABLE_LOCAL && rule->table != RT6_TABLE_MAIN)
		return false;
	return true;
}