static int evdev_open(struct inode *inode, struct file *file)
{
	struct evdev *evdev = container_of(inode->i_cdev, struct evdev, cdev);
	unsigned int bufsize = evdev_compute_buffer_size(evdev->handle.dev);
	unsigned int size = sizeof(struct evdev_client) +
					bufsize * sizeof(struct input_event);
	struct evdev_client *client;
	int error;

	client = kzalloc(size, GFP_KERNEL | __GFP_NOWARN);
	if (!client)
		client = vzalloc(size);
	if (!client)
		return -ENOMEM;

	client->bufsize = bufsize;
	spin_lock_init(&client->buffer_lock);
	client->evdev = evdev;
	evdev_attach_client(evdev, client);

	error = evdev_open_device(evdev);
	if (error)
		goto err_free_client;

	file->private_data = client;
	nonseekable_open(inode, file);

	return 0;

 err_free_client:
	evdev_detach_client(evdev, client);
	kfree(client);
	return error;
}