void amba_device_put(struct amba_device *dev)
{
	put_device(&dev->dev);
}