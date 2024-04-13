static void evdev_disconnect(struct input_handle *handle)
{
	struct evdev *evdev = handle->private;

	device_del(&evdev->dev);
	evdev_cleanup(evdev);
	input_free_minor(MINOR(evdev->dev.devt));
	input_unregister_handle(handle);
	put_device(&evdev->dev);
}