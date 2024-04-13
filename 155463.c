int amba_request_regions(struct amba_device *dev, const char *name)
{
	int ret = 0;
	u32 size;

	if (!name)
		name = dev->dev.driver->name;

	size = resource_size(&dev->res);

	if (!request_mem_region(dev->res.start, size, name))
		ret = -EBUSY;

	return ret;
}