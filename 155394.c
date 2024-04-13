static void vhost_net_exit(void)
{
	misc_deregister(&vhost_net_misc);
}