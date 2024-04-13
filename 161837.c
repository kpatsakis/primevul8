static int __init selinux_nf_ip_init(void)
{
	int err = 0;

	if (!selinux_enabled)
		goto out;

	printk(KERN_DEBUG "SELinux:  Registering netfilter hooks\n");

	err = nf_register_hooks(selinux_ipv4_ops, ARRAY_SIZE(selinux_ipv4_ops));
	if (err)
		panic("SELinux: nf_register_hooks for IPv4: error %d\n", err);

#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	err = nf_register_hooks(selinux_ipv6_ops, ARRAY_SIZE(selinux_ipv6_ops));
	if (err)
		panic("SELinux: nf_register_hooks for IPv6: error %d\n", err);
#endif	/* IPV6 */

out:
	return err;
}