int amba_device_register(struct amba_device *dev, struct resource *parent)
{
	amba_device_initialize(dev, dev->dev.init_name);
	dev->dev.init_name = NULL;

	return amba_device_add(dev, parent);
}