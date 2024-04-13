static void evdev_free(struct device *dev)
{
	struct evdev *evdev = container_of(dev, struct evdev, dev);

	input_put_device(evdev->handle.dev);
	kfree(evdev);
}