static void selinux_nf_ip_exit(void)
{
	printk(KERN_DEBUG "SELinux:  Unregistering netfilter hooks\n");

	nf_unregister_hooks(selinux_ipv4_ops, ARRAY_SIZE(selinux_ipv4_ops));
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
	nf_unregister_hooks(selinux_ipv6_ops, ARRAY_SIZE(selinux_ipv6_ops));
#endif	/* IPV6 */
}