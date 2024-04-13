static int evdev_handle_set_keycode(struct input_dev *dev, void __user *p)
{
	struct input_keymap_entry ke = {
		.len	= sizeof(unsigned int),
		.flags	= 0,
	};
	int __user *ip = (int __user *)p;

	if (copy_from_user(ke.scancode, p, sizeof(unsigned int)))
		return -EFAULT;

	if (get_user(ke.keycode, ip + 1))
		return -EFAULT;

	return input_set_keycode(dev, &ke);
}