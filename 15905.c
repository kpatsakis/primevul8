static int can_idmap_mount(const struct mount_kattr *kattr, struct mount *mnt)
{
	struct vfsmount *m = &mnt->mnt;

	if (!kattr->mnt_userns)
		return 0;

	/*
	 * Once a mount has been idmapped we don't allow it to change its
	 * mapping. It makes things simpler and callers can just create
	 * another bind-mount they can idmap if they want to.
	 */
	if (mnt_user_ns(m) != &init_user_ns)
		return -EPERM;

	/* The underlying filesystem doesn't support idmapped mounts yet. */
	if (!(m->mnt_sb->s_type->fs_flags & FS_ALLOW_IDMAP))
		return -EINVAL;

	/* Don't yet support filesystem mountable in user namespaces. */
	if (m->mnt_sb->s_user_ns != &init_user_ns)
		return -EINVAL;

	/* We're not controlling the superblock. */
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	/* Mount has already been visible in the filesystem hierarchy. */
	if (!is_anon_ns(mnt->mnt_ns))
		return -EINVAL;

	return 0;
}