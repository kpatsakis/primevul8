static void kvm_exit_debug(void)
{
	struct kvm_stats_debugfs_item *p;

	for (p = debugfs_entries; p->name; ++p)
		debugfs_remove(p->dentry);
	debugfs_remove(kvm_debugfs_dir);
}