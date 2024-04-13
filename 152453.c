static int __init xenbus_init(void)
{
	int err;

	if (!xen_domain())
		return -ENODEV;

	err = misc_register(&xenbus_dev);
	if (err)
		pr_err("Could not register xenbus frontend device\n");
	return err;
}