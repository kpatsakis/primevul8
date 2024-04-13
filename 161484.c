static struct workqueue_struct *dev_to_wq(struct device *dev)
{
	struct wq_device *wq_dev = container_of(dev, struct wq_device, dev);

	return wq_dev->wq;
}