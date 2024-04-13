static int __init ucma_init(void)
{
	int ret;

	ret = misc_register(&ucma_misc);
	if (ret)
		return ret;

	ret = device_create_file(ucma_misc.this_device, &dev_attr_abi_version);
	if (ret) {
		pr_err("rdma_ucm: couldn't create abi_version attr\n");
		goto err1;
	}

	ucma_ctl_table_hdr = register_net_sysctl(&init_net, "net/rdma_ucm", ucma_ctl_table);
	if (!ucma_ctl_table_hdr) {
		pr_err("rdma_ucm: couldn't register sysctl paths\n");
		ret = -ENOMEM;
		goto err2;
	}

	ret = ib_register_client(&rdma_cma_client);
	if (ret)
		goto err3;

	return 0;
err3:
	unregister_net_sysctl_table(ucma_ctl_table_hdr);
err2:
	device_remove_file(ucma_misc.this_device, &dev_attr_abi_version);
err1:
	misc_deregister(&ucma_misc);
	return ret;
}