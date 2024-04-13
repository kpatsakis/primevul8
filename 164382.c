static int evdev_release(struct inode *inode, struct file *file)
{
	struct evdev_client *client = file->private_data;
	struct evdev *evdev = client->evdev;

	mutex_lock(&evdev->mutex);
	evdev_ungrab(evdev, client);
	mutex_unlock(&evdev->mutex);

	evdev_detach_client(evdev, client);

	if (is_vmalloc_addr(client))
		vfree(client);
	else
		kfree(client);

	evdev_close_device(evdev);

	return 0;
}