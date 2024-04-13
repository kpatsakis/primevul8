static unsigned int attr_flags_to_mnt_flags(u64 attr_flags)
{
	unsigned int mnt_flags = 0;

	if (attr_flags & MOUNT_ATTR_RDONLY)
		mnt_flags |= MNT_READONLY;
	if (attr_flags & MOUNT_ATTR_NOSUID)
		mnt_flags |= MNT_NOSUID;
	if (attr_flags & MOUNT_ATTR_NODEV)
		mnt_flags |= MNT_NODEV;
	if (attr_flags & MOUNT_ATTR_NOEXEC)
		mnt_flags |= MNT_NOEXEC;
	if (attr_flags & MOUNT_ATTR_NODIRATIME)
		mnt_flags |= MNT_NODIRATIME;
	if (attr_flags & MOUNT_ATTR_NOSYMFOLLOW)
		mnt_flags |= MNT_NOSYMFOLLOW;

	return mnt_flags;
}