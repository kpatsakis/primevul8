void amba_device_unregister(struct amba_device *dev)
{
	device_unregister(&dev->dev);
}