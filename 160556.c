__init int net_sysctl_init(void)
{
	static struct ctl_table empty[1];
	int ret = -ENOMEM;
	/* Avoid limitations in the sysctl implementation by
	 * registering "/proc/sys/net" as an empty directory not in a
	 * network namespace.
	 */
	net_header = register_sysctl("net", empty);
	if (!net_header)
		goto out;
	ret = register_pernet_subsys(&sysctl_pernet_ops);
	if (ret)
		goto out;
	register_sysctl_root(&net_sysctl_root);
out:
	return ret;
}