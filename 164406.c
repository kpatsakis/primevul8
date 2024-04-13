static unsigned int evdev_poll(struct file *file, poll_table *wait)
{
	struct evdev_client *client = file->private_data;
	struct evdev *evdev = client->evdev;
	unsigned int mask;

	poll_wait(file, &evdev->wait, wait);

	if (evdev->exist && !client->revoked)
		mask = POLLOUT | POLLWRNORM;
	else
		mask = POLLHUP | POLLERR;

	if (client->packet_head != client->tail)
		mask |= POLLIN | POLLRDNORM;

	return mask;
}