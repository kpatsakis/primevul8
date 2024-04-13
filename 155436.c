int amba_device_add(struct amba_device *dev, struct resource *parent)
{
	int ret = amba_device_try_add(dev, parent);

	if (ret == -EPROBE_DEFER) {
		struct deferred_device *ddev;

		ddev = kmalloc(sizeof(*ddev), GFP_KERNEL);
		if (!ddev)
			return -ENOMEM;

		ddev->dev = dev;
		ddev->parent = parent;
		ret = 0;

		mutex_lock(&deferred_devices_lock);

		if (list_empty(&deferred_devices))
			schedule_delayed_work(&deferred_retry_work,
					      DEFERRED_DEVICE_TIMEOUT);
		list_add_tail(&ddev->node, &deferred_devices);

		mutex_unlock(&deferred_devices_lock);
	}
	return ret;
}