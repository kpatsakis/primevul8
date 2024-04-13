SYSCALL_DEFINE2(pivot_root, const char __user *, new_root,
		const char __user *, put_old)
{
	struct path new, old, root;
	struct mount *new_mnt, *root_mnt, *old_mnt, *root_parent, *ex_parent;
	struct mountpoint *old_mp, *root_mp;
	int error;

	if (!may_mount())
		return -EPERM;

	error = user_path_at(AT_FDCWD, new_root,
			     LOOKUP_FOLLOW | LOOKUP_DIRECTORY, &new);
	if (error)
		goto out0;

	error = user_path_at(AT_FDCWD, put_old,
			     LOOKUP_FOLLOW | LOOKUP_DIRECTORY, &old);
	if (error)
		goto out1;

	error = security_sb_pivotroot(&old, &new);
	if (error)
		goto out2;

	get_fs_root(current->fs, &root);
	old_mp = lock_mount(&old);
	error = PTR_ERR(old_mp);
	if (IS_ERR(old_mp))
		goto out3;

	error = -EINVAL;
	new_mnt = real_mount(new.mnt);
	root_mnt = real_mount(root.mnt);
	old_mnt = real_mount(old.mnt);
	ex_parent = new_mnt->mnt_parent;
	root_parent = root_mnt->mnt_parent;
	if (IS_MNT_SHARED(old_mnt) ||
		IS_MNT_SHARED(ex_parent) ||
		IS_MNT_SHARED(root_parent))
		goto out4;
	if (!check_mnt(root_mnt) || !check_mnt(new_mnt))
		goto out4;
	if (new_mnt->mnt.mnt_flags & MNT_LOCKED)
		goto out4;
	error = -ENOENT;
	if (d_unlinked(new.dentry))
		goto out4;
	error = -EBUSY;
	if (new_mnt == root_mnt || old_mnt == root_mnt)
		goto out4; /* loop, on the same file system  */
	error = -EINVAL;
	if (root.mnt->mnt_root != root.dentry)
		goto out4; /* not a mountpoint */
	if (!mnt_has_parent(root_mnt))
		goto out4; /* not attached */
	if (new.mnt->mnt_root != new.dentry)
		goto out4; /* not a mountpoint */
	if (!mnt_has_parent(new_mnt))
		goto out4; /* not attached */
	/* make sure we can reach put_old from new_root */
	if (!is_path_reachable(old_mnt, old.dentry, &new))
		goto out4;
	/* make certain new is below the root */
	if (!is_path_reachable(new_mnt, new.dentry, &root))
		goto out4;
	lock_mount_hash();
	umount_mnt(new_mnt);
	root_mp = unhash_mnt(root_mnt);  /* we'll need its mountpoint */
	if (root_mnt->mnt.mnt_flags & MNT_LOCKED) {
		new_mnt->mnt.mnt_flags |= MNT_LOCKED;
		root_mnt->mnt.mnt_flags &= ~MNT_LOCKED;
	}
	/* mount old root on put_old */
	attach_mnt(root_mnt, old_mnt, old_mp);
	/* mount new_root on / */
	attach_mnt(new_mnt, root_parent, root_mp);
	mnt_add_count(root_parent, -1);
	touch_mnt_namespace(current->nsproxy->mnt_ns);
	/* A moved mount should not expire automatically */
	list_del_init(&new_mnt->mnt_expire);
	put_mountpoint(root_mp);
	unlock_mount_hash();
	chroot_fs_refs(&root, &new);
	error = 0;
out4:
	unlock_mount(old_mp);
	if (!error)
		mntput_no_expire(ex_parent);
out3:
	path_put(&root);
out2:
	path_put(&old);
out1:
	path_put(&new);
out0:
	return error;
}