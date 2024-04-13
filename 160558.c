void unregister_net_sysctl_table(struct ctl_table_header *header)
{
	unregister_sysctl_table(header);
}