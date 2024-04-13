static int ksys_umount(char __user *name, int flags)
{
	int lookup_flags = LOOKUP_MOUNTPOINT;
	struct path path;
	int ret;

	// basic validity checks done first
	if (flags & ~(MNT_FORCE | MNT_DETACH | MNT_EXPIRE | UMOUNT_NOFOLLOW))
		return -EINVAL;

	if (!(flags & UMOUNT_NOFOLLOW))
		lookup_flags |= LOOKUP_FOLLOW;
	ret = user_path_at(AT_FDCWD, name, lookup_flags, &path);
	if (ret)
		return ret;
	return path_umount(&path, flags);
}