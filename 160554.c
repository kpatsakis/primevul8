static void __net_exit sysctl_net_exit(struct net *net)
{
	retire_sysctl_set(&net->sysctls);
}