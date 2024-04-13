void hid_debug_exit(void)
{
	debugfs_remove_recursive(hid_debug_root);
}