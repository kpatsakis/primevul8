static int evdev_connect(struct input_handler *handler, struct input_dev *dev,
			 const struct input_device_id *id)
{
	struct evdev *evdev;
	int minor;
	int dev_no;
	int error;

	minor = input_get_new_minor(EVDEV_MINOR_BASE, EVDEV_MINORS, true);
	if (minor < 0) {
		error = minor;
		pr_err("failed to reserve new minor: %d\n", error);
		return error;
	}

	evdev = kzalloc(sizeof(struct evdev), GFP_KERNEL);
	if (!evdev) {
		error = -ENOMEM;
		goto err_free_minor;
	}

	INIT_LIST_HEAD(&evdev->client_list);
	spin_lock_init(&evdev->client_lock);
	mutex_init(&evdev->mutex);
	init_waitqueue_head(&evdev->wait);
	evdev->exist = true;

	dev_no = minor;
	/* Normalize device number if it falls into legacy range */
	if (dev_no < EVDEV_MINOR_BASE + EVDEV_MINORS)
		dev_no -= EVDEV_MINOR_BASE;
	dev_set_name(&evdev->dev, "event%d", dev_no);

	evdev->handle.dev = input_get_device(dev);
	evdev->handle.name = dev_name(&evdev->dev);
	evdev->handle.handler = handler;
	evdev->handle.private = evdev;

	evdev->dev.devt = MKDEV(INPUT_MAJOR, minor);
	evdev->dev.class = &input_class;
	evdev->dev.parent = &dev->dev;
	evdev->dev.release = evdev_free;
	device_initialize(&evdev->dev);

	error = input_register_handle(&evdev->handle);
	if (error)
		goto err_free_evdev;

	cdev_init(&evdev->cdev, &evdev_fops);
	evdev->cdev.kobj.parent = &evdev->dev.kobj;
	error = cdev_add(&evdev->cdev, evdev->dev.devt, 1);
	if (error)
		goto err_unregister_handle;

	error = device_add(&evdev->dev);
	if (error)
		goto err_cleanup_evdev;

	return 0;

 err_cleanup_evdev:
	evdev_cleanup(evdev);
 err_unregister_handle:
	input_unregister_handle(&evdev->handle);
 err_free_evdev:
	put_device(&evdev->dev);
 err_free_minor:
	input_free_minor(minor);
	return error;
}