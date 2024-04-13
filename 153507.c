void hid_debug_init(void)
{
	hid_debug_root = debugfs_create_dir("hid", NULL);
}