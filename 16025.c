SYSCALL_DEFINE5(move_mount,
		int, from_dfd, const char __user *, from_pathname,
		int, to_dfd, const char __user *, to_pathname,
		unsigned int, flags)
{
	struct path from_path, to_path;
	unsigned int lflags;
	int ret = 0;

	if (!may_mount())
		return -EPERM;

	if (flags & ~MOVE_MOUNT__MASK)
		return -EINVAL;

	/* If someone gives a pathname, they aren't permitted to move
	 * from an fd that requires unmount as we can't get at the flag
	 * to clear it afterwards.
	 */
	lflags = 0;
	if (flags & MOVE_MOUNT_F_SYMLINKS)	lflags |= LOOKUP_FOLLOW;
	if (flags & MOVE_MOUNT_F_AUTOMOUNTS)	lflags |= LOOKUP_AUTOMOUNT;
	if (flags & MOVE_MOUNT_F_EMPTY_PATH)	lflags |= LOOKUP_EMPTY;

	ret = user_path_at(from_dfd, from_pathname, lflags, &from_path);
	if (ret < 0)
		return ret;

	lflags = 0;
	if (flags & MOVE_MOUNT_T_SYMLINKS)	lflags |= LOOKUP_FOLLOW;
	if (flags & MOVE_MOUNT_T_AUTOMOUNTS)	lflags |= LOOKUP_AUTOMOUNT;
	if (flags & MOVE_MOUNT_T_EMPTY_PATH)	lflags |= LOOKUP_EMPTY;

	ret = user_path_at(to_dfd, to_pathname, lflags, &to_path);
	if (ret < 0)
		goto out_from;

	ret = security_move_mount(&from_path, &to_path);
	if (ret < 0)
		goto out_to;

	ret = do_move_mount(&from_path, &to_path);

out_to:
	path_put(&to_path);
out_from:
	path_put(&from_path);
	return ret;
}