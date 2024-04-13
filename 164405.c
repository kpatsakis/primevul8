static int evdev_grab(struct evdev *evdev, struct evdev_client *client)
{
	int error;

	if (evdev->grab)
		return -EBUSY;

	error = input_grab_device(&evdev->handle);
	if (error)
		return error;

	rcu_assign_pointer(evdev->grab, client);

	return 0;
}