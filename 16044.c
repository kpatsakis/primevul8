SYSCALL_DEFINE5(mount_setattr, int, dfd, const char __user *, path,
		unsigned int, flags, struct mount_attr __user *, uattr,
		size_t, usize)
{
	int err;
	struct path target;
	struct mount_attr attr;
	struct mount_kattr kattr;

	BUILD_BUG_ON(sizeof(struct mount_attr) != MOUNT_ATTR_SIZE_VER0);

	if (flags & ~(AT_EMPTY_PATH |
		      AT_RECURSIVE |
		      AT_SYMLINK_NOFOLLOW |
		      AT_NO_AUTOMOUNT))
		return -EINVAL;

	if (unlikely(usize > PAGE_SIZE))
		return -E2BIG;
	if (unlikely(usize < MOUNT_ATTR_SIZE_VER0))
		return -EINVAL;

	if (!may_mount())
		return -EPERM;

	err = copy_struct_from_user(&attr, sizeof(attr), uattr, usize);
	if (err)
		return err;

	/* Don't bother walking through the mounts if this is a nop. */
	if (attr.attr_set == 0 &&
	    attr.attr_clr == 0 &&
	    attr.propagation == 0)
		return 0;

	err = build_mount_kattr(&attr, usize, &kattr, flags);
	if (err)
		return err;

	err = user_path_at(dfd, path, kattr.lookup_flags, &target);
	if (err)
		return err;

	err = do_mount_setattr(&target, &kattr);
	finish_mount_kattr(&kattr);
	path_put(&target);
	return err;
}