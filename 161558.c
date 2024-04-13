static void wq_device_release(struct device *dev)
{
	struct wq_device *wq_dev = container_of(dev, struct wq_device, dev);

	kfree(wq_dev);
}