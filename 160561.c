struct ctl_table_header *register_net_sysctl(struct net *net,
	const char *path, struct ctl_table *table)
{
	return __register_sysctl_table(&net->sysctls, path, table);
}