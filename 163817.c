static void __exit ppp_cleanup(void)
{
	/* should never happen */
	if (atomic_read(&ppp_unit_count) || atomic_read(&channel_count))
		printk(KERN_ERR "PPP: removing module but units remain!\n");
	unregister_chrdev(PPP_MAJOR, "ppp");
	device_destroy(ppp_class, MKDEV(PPP_MAJOR, 0));
	class_destroy(ppp_class);
	unregister_pernet_gen_device(ppp_net_id, &ppp_net_ops);
}