void __detach_mounts(struct dentry *dentry)
{
	struct mountpoint *mp;
	struct mount *mnt;

	namespace_lock();
	lock_mount_hash();
	mp = lookup_mountpoint(dentry);
	if (!mp)
		goto out_unlock;

	event++;
	while (!hlist_empty(&mp->m_list)) {
		mnt = hlist_entry(mp->m_list.first, struct mount, mnt_mp_list);
		if (mnt->mnt.mnt_flags & MNT_UMOUNT) {
			umount_mnt(mnt);
			hlist_add_head(&mnt->mnt_umount, &unmounted);
		}
		else umount_tree(mnt, UMOUNT_CONNECTED);
	}
	put_mountpoint(mp);
out_unlock:
	unlock_mount_hash();
	namespace_unlock();
}