static int __init wq_sysfs_init(void)
{
	int err;

	err = subsys_virtual_register(&wq_subsys, NULL);
	if (err)
		return err;

	return device_create_file(wq_subsys.dev_root, &wq_sysfs_cpumask_attr);
}