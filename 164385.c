static long evdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	return evdev_ioctl_handler(file, cmd, (void __user *)arg, 0);
}