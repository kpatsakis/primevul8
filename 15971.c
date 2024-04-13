static int do_add_mount(struct mount *newmnt, struct mountpoint *mp,
			struct path *path, int mnt_flags)
{
	struct mount *parent = real_mount(path->mnt);

	mnt_flags &= ~MNT_INTERNAL_FLAGS;

	if (unlikely(!check_mnt(parent))) {
		/* that's acceptable only for automounts done in private ns */
		if (!(mnt_flags & MNT_SHRINKABLE))
			return -EINVAL;
		/* ... and for those we'd better have mountpoint still alive */
		if (!parent->mnt_ns)
			return -EINVAL;
	}

	/* Refuse the same filesystem on the same mount point */
	if (path->mnt->mnt_sb == newmnt->mnt.mnt_sb &&
	    path->mnt->mnt_root == path->dentry)
		return -EBUSY;

	if (d_is_symlink(newmnt->mnt.mnt_root))
		return -EINVAL;

	newmnt->mnt.mnt_flags = mnt_flags;
	return graft_tree(newmnt, parent, mp);
}