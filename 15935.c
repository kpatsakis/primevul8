static int do_reconfigure_mnt(struct path *path, unsigned int mnt_flags)
{
	struct super_block *sb = path->mnt->mnt_sb;
	struct mount *mnt = real_mount(path->mnt);
	int ret;

	if (!check_mnt(mnt))
		return -EINVAL;

	if (path->dentry != mnt->mnt.mnt_root)
		return -EINVAL;

	if (!can_change_locked_flags(mnt, mnt_flags))
		return -EPERM;

	/*
	 * We're only checking whether the superblock is read-only not
	 * changing it, so only take down_read(&sb->s_umount).
	 */
	down_read(&sb->s_umount);
	lock_mount_hash();
	ret = change_mount_ro_state(mnt, mnt_flags);
	if (ret == 0)
		set_mount_attributes(mnt, mnt_flags);
	unlock_mount_hash();
	up_read(&sb->s_umount);

	mnt_warn_timestamp_expiry(path, &mnt->mnt);

	return ret;
}