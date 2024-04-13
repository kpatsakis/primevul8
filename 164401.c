static int evdev_fasync(int fd, struct file *file, int on)
{
	struct evdev_client *client = file->private_data;

	return fasync_helper(fd, file, on, &client->fasync);
}