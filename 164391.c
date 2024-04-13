static int evdev_handle_get_keycode_v2(struct input_dev *dev, void __user *p)
{
	struct input_keymap_entry ke;
	int error;

	if (copy_from_user(&ke, p, sizeof(ke)))
		return -EFAULT;

	error = input_get_keycode(dev, &ke);
	if (error)
		return error;

	if (copy_to_user(p, &ke, sizeof(ke)))
		return -EFAULT;

	return 0;
}