static long evdev_ioctl_compat(struct file *file,
				unsigned int cmd, unsigned long arg)
{
	return evdev_ioctl_handler(file, cmd, compat_ptr(arg), 1);
}