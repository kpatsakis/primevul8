int finish_automount(struct vfsmount *m, struct path *path)
{
	struct dentry *dentry = path->dentry;
	struct mountpoint *mp;
	struct mount *mnt;
	int err;

	if (!m)
		return 0;
	if (IS_ERR(m))
		return PTR_ERR(m);

	mnt = real_mount(m);
	/* The new mount record should have at least 2 refs to prevent it being
	 * expired before we get a chance to add it
	 */
	BUG_ON(mnt_get_count(mnt) < 2);

	if (m->mnt_sb == path->mnt->mnt_sb &&
	    m->mnt_root == dentry) {
		err = -ELOOP;
		goto discard;
	}

	/*
	 * we don't want to use lock_mount() - in this case finding something
	 * that overmounts our mountpoint to be means "quitely drop what we've
	 * got", not "try to mount it on top".
	 */
	inode_lock(dentry->d_inode);
	namespace_lock();
	if (unlikely(cant_mount(dentry))) {
		err = -ENOENT;
		goto discard_locked;
	}
	rcu_read_lock();
	if (unlikely(__lookup_mnt(path->mnt, dentry))) {
		rcu_read_unlock();
		err = 0;
		goto discard_locked;
	}
	rcu_read_unlock();
	mp = get_mountpoint(dentry);
	if (IS_ERR(mp)) {
		err = PTR_ERR(mp);
		goto discard_locked;
	}

	err = do_add_mount(mnt, mp, path, path->mnt->mnt_flags | MNT_SHRINKABLE);
	unlock_mount(mp);
	if (unlikely(err))
		goto discard;
	mntput(m);
	return 0;

discard_locked:
	namespace_unlock();
	inode_unlock(dentry->d_inode);
discard:
	/* remove m from any expiration list it may be on */
	if (!list_empty(&mnt->mnt_expire)) {
		namespace_lock();
		list_del_init(&mnt->mnt_expire);
		namespace_unlock();
	}
	mntput(m);
	mntput(m);
	return err;
}