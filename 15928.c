static int do_move_mount(struct path *old_path, struct path *new_path)
{
	struct mnt_namespace *ns;
	struct mount *p;
	struct mount *old;
	struct mount *parent;
	struct mountpoint *mp, *old_mp;
	int err;
	bool attached;

	mp = lock_mount(new_path);
	if (IS_ERR(mp))
		return PTR_ERR(mp);

	old = real_mount(old_path->mnt);
	p = real_mount(new_path->mnt);
	parent = old->mnt_parent;
	attached = mnt_has_parent(old);
	old_mp = old->mnt_mp;
	ns = old->mnt_ns;

	err = -EINVAL;
	/* The mountpoint must be in our namespace. */
	if (!check_mnt(p))
		goto out;

	/* The thing moved must be mounted... */
	if (!is_mounted(&old->mnt))
		goto out;

	/* ... and either ours or the root of anon namespace */
	if (!(attached ? check_mnt(old) : is_anon_ns(ns)))
		goto out;

	if (old->mnt.mnt_flags & MNT_LOCKED)
		goto out;

	if (old_path->dentry != old_path->mnt->mnt_root)
		goto out;

	if (d_is_dir(new_path->dentry) !=
	    d_is_dir(old_path->dentry))
		goto out;
	/*
	 * Don't move a mount residing in a shared parent.
	 */
	if (attached && IS_MNT_SHARED(parent))
		goto out;
	/*
	 * Don't move a mount tree containing unbindable mounts to a destination
	 * mount which is shared.
	 */
	if (IS_MNT_SHARED(p) && tree_contains_unbindable(old))
		goto out;
	err = -ELOOP;
	if (!check_for_nsfs_mounts(old))
		goto out;
	for (; mnt_has_parent(p); p = p->mnt_parent)
		if (p == old)
			goto out;

	err = attach_recursive_mnt(old, real_mount(new_path->mnt), mp,
				   attached);
	if (err)
		goto out;

	/* if the mount is moved, it should no longer be expire
	 * automatically */
	list_del_init(&old->mnt_expire);
	if (attached)
		put_mountpoint(old_mp);
out:
	unlock_mount(mp);
	if (!err) {
		if (attached)
			mntput_no_expire(parent);
		else
			free_mnt_ns(ns);
	}
	return err;
}