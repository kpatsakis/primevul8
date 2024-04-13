struct dentry *mount_subtree(struct vfsmount *m, const char *name)
{
	struct mount *mnt = real_mount(m);
	struct mnt_namespace *ns;
	struct super_block *s;
	struct path path;
	int err;

	ns = alloc_mnt_ns(&init_user_ns, true);
	if (IS_ERR(ns)) {
		mntput(m);
		return ERR_CAST(ns);
	}
	mnt->mnt_ns = ns;
	ns->root = mnt;
	ns->mounts++;
	list_add(&mnt->mnt_list, &ns->list);

	err = vfs_path_lookup(m->mnt_root, m,
			name, LOOKUP_FOLLOW|LOOKUP_AUTOMOUNT, &path);

	put_mnt_ns(ns);

	if (err)
		return ERR_PTR(err);

	/* trade a vfsmount reference for active sb one */
	s = path.mnt->mnt_sb;
	atomic_inc(&s->s_active);
	mntput(path.mnt);
	/* lock the sucker */
	down_write(&s->s_umount);
	/* ... and return the root of (sub)tree on it */
	return path.dentry;
}