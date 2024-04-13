static int amba_remove(struct device *dev)
{
	struct amba_device *pcdev = to_amba_device(dev);
	struct amba_driver *drv = to_amba_driver(dev->driver);
	int ret;

	pm_runtime_get_sync(dev);
	ret = drv->remove(pcdev);
	pm_runtime_put_noidle(dev);

	/* Undo the runtime PM settings in amba_probe() */
	pm_runtime_disable(dev);
	pm_runtime_set_suspended(dev);
	pm_runtime_put_noidle(dev);

	amba_put_disable_pclk(pcdev);
	dev_pm_domain_detach(dev, true);

	return ret;
}