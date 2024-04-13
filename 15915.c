static struct mount *__do_loopback(struct path *old_path, int recurse)
{
	struct mount *mnt = ERR_PTR(-EINVAL), *old = real_mount(old_path->mnt);

	if (IS_MNT_UNBINDABLE(old))
		return mnt;

	if (!check_mnt(old) && old_path->dentry->d_op != &ns_dentry_operations)
		return mnt;

	if (!recurse && has_locked_children(old, old_path->dentry))
		return mnt;

	if (recurse)
		mnt = copy_tree(old, old_path->dentry, CL_COPY_MNT_NS_FILE);
	else
		mnt = clone_mnt(old, old_path->dentry, 0);

	if (!IS_ERR(mnt))
		mnt->mnt.mnt_flags &= ~MNT_LOCKED;

	return mnt;
}