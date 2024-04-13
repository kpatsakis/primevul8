static int do_loopback(struct path *path, const char *old_name,
				int recurse)
{
	struct path old_path;
	struct mount *mnt = NULL, *parent;
	struct mountpoint *mp;
	int err;
	if (!old_name || !*old_name)
		return -EINVAL;
	err = kern_path(old_name, LOOKUP_FOLLOW|LOOKUP_AUTOMOUNT, &old_path);
	if (err)
		return err;

	err = -EINVAL;
	if (mnt_ns_loop(old_path.dentry))
		goto out;

	mp = lock_mount(path);
	if (IS_ERR(mp)) {
		err = PTR_ERR(mp);
		goto out;
	}

	parent = real_mount(path->mnt);
	if (!check_mnt(parent))
		goto out2;

	mnt = __do_loopback(&old_path, recurse);
	if (IS_ERR(mnt)) {
		err = PTR_ERR(mnt);
		goto out2;
	}

	err = graft_tree(mnt, parent, mp);
	if (err) {
		lock_mount_hash();
		umount_tree(mnt, UMOUNT_SYNC);
		unlock_mount_hash();
	}
out2:
	unlock_mount(mp);
out:
	path_put(&old_path);
	return err;
}