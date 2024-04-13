vfs_submount(const struct dentry *mountpoint, struct file_system_type *type,
	     const char *name, void *data)
{
	/* Until it is worked out how to pass the user namespace
	 * through from the parent mount to the submount don't support
	 * unprivileged mounts with submounts.
	 */
	if (mountpoint->d_sb->s_user_ns != &init_user_ns)
		return ERR_PTR(-EPERM);

	return vfs_kern_mount(type, SB_SUBMOUNT, name, data);
}