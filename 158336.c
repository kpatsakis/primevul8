static int hiddev_open(struct inode *inode, struct file *file)
{
	struct hiddev_list *list;
	struct usb_interface *intf;
	struct hid_device *hid;
	struct hiddev *hiddev;
	int res;

	intf = usbhid_find_interface(iminor(inode));
	if (!intf)
		return -ENODEV;
	hid = usb_get_intfdata(intf);
	hiddev = hid->hiddev;

	if (!(list = vzalloc(sizeof(struct hiddev_list))))
		return -ENOMEM;
	mutex_init(&list->thread_lock);
	list->hiddev = hiddev;
	file->private_data = list;

	/*
	 * no need for locking because the USB major number
	 * is shared which usbcore guards against disconnect
	 */
	if (list->hiddev->exist) {
		if (!list->hiddev->open++) {
			res = hid_hw_open(hiddev->hid);
			if (res < 0)
				goto bail;
		}
	} else {
		res = -ENODEV;
		goto bail;
	}

	spin_lock_irq(&list->hiddev->list_lock);
	list_add_tail(&list->node, &hiddev->list);
	spin_unlock_irq(&list->hiddev->list_lock);

	mutex_lock(&hiddev->existancelock);
	/*
	 * recheck exist with existance lock held to
	 * avoid opening a disconnected device
	 */
	if (!list->hiddev->exist) {
		res = -ENODEV;
		goto bail_unlock;
	}
	if (!list->hiddev->open++)
		if (list->hiddev->exist) {
			struct hid_device *hid = hiddev->hid;
			res = hid_hw_power(hid, PM_HINT_FULLON);
			if (res < 0)
				goto bail_unlock;
			res = hid_hw_open(hid);
			if (res < 0)
				goto bail_normal_power;
		}
	mutex_unlock(&hiddev->existancelock);
	return 0;
bail_normal_power:
	hid_hw_power(hid, PM_HINT_NORMAL);
bail_unlock:
	mutex_unlock(&hiddev->existancelock);

	spin_lock_irq(&list->hiddev->list_lock);
	list_del(&list->node);
	spin_unlock_irq(&list->hiddev->list_lock);
bail:
	file->private_data = NULL;
	vfree(list);
	return res;
}