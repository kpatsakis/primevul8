static void amba_device_release(struct device *dev)
{
	struct amba_device *d = to_amba_device(dev);

	if (d->res.parent)
		release_resource(&d->res);
	kfree(d);
}