void hid_debug_register(struct hid_device *hdev, const char *name)
{
	hdev->debug_dir = debugfs_create_dir(name, hid_debug_root);
	hdev->debug_rdesc = debugfs_create_file("rdesc", 0400,
			hdev->debug_dir, hdev, &hid_debug_rdesc_fops);
	hdev->debug_events = debugfs_create_file("events", 0400,
			hdev->debug_dir, hdev, &hid_debug_events_fops);
	hdev->debug = 1;
}