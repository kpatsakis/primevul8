static int amba_match(struct device *dev, struct device_driver *drv)
{
	struct amba_device *pcdev = to_amba_device(dev);
	struct amba_driver *pcdrv = to_amba_driver(drv);

	/* When driver_override is set, only bind to the matching driver */
	if (pcdev->driver_override)
		return !strcmp(pcdev->driver_override, drv->name);

	return amba_lookup(pcdrv->id_table, pcdev) != NULL;
}