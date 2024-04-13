SYSCALL_DEFINE3(open_tree, int, dfd, const char __user *, filename, unsigned, flags)
{
	struct file *file;
	struct path path;
	int lookup_flags = LOOKUP_AUTOMOUNT | LOOKUP_FOLLOW;
	bool detached = flags & OPEN_TREE_CLONE;
	int error;
	int fd;

	BUILD_BUG_ON(OPEN_TREE_CLOEXEC != O_CLOEXEC);

	if (flags & ~(AT_EMPTY_PATH | AT_NO_AUTOMOUNT | AT_RECURSIVE |
		      AT_SYMLINK_NOFOLLOW | OPEN_TREE_CLONE |
		      OPEN_TREE_CLOEXEC))
		return -EINVAL;

	if ((flags & (AT_RECURSIVE | OPEN_TREE_CLONE)) == AT_RECURSIVE)
		return -EINVAL;

	if (flags & AT_NO_AUTOMOUNT)
		lookup_flags &= ~LOOKUP_AUTOMOUNT;
	if (flags & AT_SYMLINK_NOFOLLOW)
		lookup_flags &= ~LOOKUP_FOLLOW;
	if (flags & AT_EMPTY_PATH)
		lookup_flags |= LOOKUP_EMPTY;

	if (detached && !may_mount())
		return -EPERM;

	fd = get_unused_fd_flags(flags & O_CLOEXEC);
	if (fd < 0)
		return fd;

	error = user_path_at(dfd, filename, lookup_flags, &path);
	if (unlikely(error)) {
		file = ERR_PTR(error);
	} else {
		if (detached)
			file = open_detached_copy(&path, flags & AT_RECURSIVE);
		else
			file = dentry_open(&path, O_PATH, current_cred());
		path_put(&path);
	}
	if (IS_ERR(file)) {
		put_unused_fd(fd);
		return PTR_ERR(file);
	}
	fd_install(fd, file);
	return fd;
}