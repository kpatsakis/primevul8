static int amba_pm_runtime_suspend(struct device *dev)
{
	struct amba_device *pcdev = to_amba_device(dev);
	int ret = pm_generic_runtime_suspend(dev);

	if (ret == 0 && dev->driver) {
		if (pm_runtime_is_irq_safe(dev))
			clk_disable(pcdev->pclk);
		else
			clk_disable_unprepare(pcdev->pclk);
	}

	return ret;
}