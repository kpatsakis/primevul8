amba_ahb_device_add(struct device *parent, const char *name,
		    resource_size_t base, size_t size, int irq1, int irq2,
		    void *pdata, unsigned int periphid)
{
	return amba_aphb_device_add(parent, name, base, size, irq1, irq2, pdata,
				    periphid, ~0ULL, &iomem_resource);
}