struct vfsmount *kern_mount(struct file_system_type *type)
{
	struct vfsmount *mnt;
	mnt = vfs_kern_mount(type, SB_KERNMOUNT, type->name, NULL);
	if (!IS_ERR(mnt)) {
		/*
		 * it is a longterm mount, don't release mnt until
		 * we unmount before file sys is unregistered
		*/
		real_mount(mnt)->mnt_ns = MNT_NS_INTERNAL;
	}
	return mnt;
}