struct vfsmount *vfs_kern_mount(struct file_system_type *type,
				int flags, const char *name,
				void *data)
{
	struct fs_context *fc;
	struct vfsmount *mnt;
	int ret = 0;

	if (!type)
		return ERR_PTR(-EINVAL);

	fc = fs_context_for_mount(type, flags);
	if (IS_ERR(fc))
		return ERR_CAST(fc);

	if (name)
		ret = vfs_parse_fs_string(fc, "source",
					  name, strlen(name));
	if (!ret)
		ret = parse_monolithic_mount_data(fc, data);
	if (!ret)
		mnt = fc_mount(fc);
	else
		mnt = ERR_PTR(ret);

	put_fs_context(fc);
	return mnt;
}