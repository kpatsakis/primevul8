static int evdev_revoke(struct evdev *evdev, struct evdev_client *client,
			struct file *file)
{
	client->revoked = true;
	evdev_ungrab(evdev, client);
	input_flush_device(&evdev->handle, file);
	wake_up_interruptible(&evdev->wait);

	return 0;
}