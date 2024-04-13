static void amba_deferred_retry_func(struct work_struct *dummy)
{
	struct deferred_device *ddev, *tmp;

	mutex_lock(&deferred_devices_lock);

	list_for_each_entry_safe(ddev, tmp, &deferred_devices, node) {
		int ret = amba_device_try_add(ddev->dev, ddev->parent);

		if (ret == -EPROBE_DEFER)
			continue;

		list_del_init(&ddev->node);
		kfree(ddev);
	}

	if (!list_empty(&deferred_devices))
		schedule_delayed_work(&deferred_retry_work,
				      DEFERRED_DEVICE_TIMEOUT);

	mutex_unlock(&deferred_devices_lock);
}