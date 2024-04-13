static int amba_pm_runtime_resume(struct device *dev)
{
	struct amba_device *pcdev = to_amba_device(dev);
	int ret;

	if (dev->driver) {
		if (pm_runtime_is_irq_safe(dev))
			ret = clk_enable(pcdev->pclk);
		else
			ret = clk_prepare_enable(pcdev->pclk);
		/* Failure is probably fatal to the system, but... */
		if (ret)
			return ret;
	}

	return pm_generic_runtime_resume(dev);
}