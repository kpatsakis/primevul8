void hid_debug_unregister(struct hid_device *hdev)
{
	hdev->debug = 0;
	wake_up_interruptible(&hdev->debug_wait);
	debugfs_remove(hdev->debug_rdesc);
	debugfs_remove(hdev->debug_events);
	debugfs_remove(hdev->debug_dir);
}