void amba_release_regions(struct amba_device *dev)
{
	u32 size;

	size = resource_size(&dev->res);
	release_mem_region(dev->res.start, size);
}