static int is_seen(struct ctl_table_set *set)
{
	return &current->nsproxy->net_ns->sysctls == set;
}