amba_aphb_device_add(struct device *parent, const char *name,
		     resource_size_t base, size_t size, int irq1, int irq2,
		     void *pdata, unsigned int periphid, u64 dma_mask,
		     struct resource *resbase)
{
	struct amba_device *dev;
	int ret;

	dev = amba_device_alloc(name, base, size);
	if (!dev)
		return ERR_PTR(-ENOMEM);

	dev->dev.coherent_dma_mask = dma_mask;
	dev->irq[0] = irq1;
	dev->irq[1] = irq2;
	dev->periphid = periphid;
	dev->dev.platform_data = pdata;
	dev->dev.parent = parent;

	ret = amba_device_add(dev, resbase);
	if (ret) {
		amba_device_put(dev);
		return ERR_PTR(ret);
	}

	return dev;
}