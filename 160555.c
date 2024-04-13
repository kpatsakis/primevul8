net_ctl_header_lookup(struct ctl_table_root *root, struct nsproxy *namespaces)
{
	return &namespaces->net_ns->sysctls;
}