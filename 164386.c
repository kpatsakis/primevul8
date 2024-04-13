static int evdev_handle_mt_request(struct input_dev *dev,
				   unsigned int size,
				   int __user *ip)
{
	const struct input_mt *mt = dev->mt;
	unsigned int code;
	int max_slots;
	int i;

	if (get_user(code, &ip[0]))
		return -EFAULT;
	if (!mt || !input_is_mt_value(code))
		return -EINVAL;

	max_slots = (size - sizeof(__u32)) / sizeof(__s32);
	for (i = 0; i < mt->num_slots && i < max_slots; i++) {
		int value = input_mt_get_value(&mt->slots[i], code);
		if (put_user(value, &ip[1 + i]))
			return -EFAULT;
	}

	return 0;
}