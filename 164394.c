static int evdev_ungrab(struct evdev *evdev, struct evdev_client *client)
{
	struct evdev_client *grab = rcu_dereference_protected(evdev->grab,
					lockdep_is_held(&evdev->mutex));

	if (grab != client)
		return  -EINVAL;

	rcu_assign_pointer(evdev->grab, NULL);
	synchronize_rcu();
	input_release_device(&evdev->handle);

	return 0;
}