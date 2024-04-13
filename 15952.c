static void *copy_mount_options(const void __user * data)
{
	char *copy;
	unsigned left, offset;

	if (!data)
		return NULL;

	copy = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!copy)
		return ERR_PTR(-ENOMEM);

	left = copy_from_user(copy, data, PAGE_SIZE);

	/*
	 * Not all architectures have an exact copy_from_user(). Resort to
	 * byte at a time.
	 */
	offset = PAGE_SIZE - left;
	while (left) {
		char c;
		if (get_user(c, (const char __user *)data + offset))
			break;
		copy[offset] = c;
		left--;
		offset++;
	}

	if (left == PAGE_SIZE) {
		kfree(copy);
		return ERR_PTR(-EFAULT);
	}

	return copy;
}