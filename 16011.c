int __legitimize_mnt(struct vfsmount *bastard, unsigned seq)
{
	struct mount *mnt;
	if (read_seqretry(&mount_lock, seq))
		return 1;
	if (bastard == NULL)
		return 0;
	mnt = real_mount(bastard);
	mnt_add_count(mnt, 1);
	smp_mb();			// see mntput_no_expire()
	if (likely(!read_seqretry(&mount_lock, seq)))
		return 0;
	if (bastard->mnt_flags & MNT_SYNC_UMOUNT) {
		mnt_add_count(mnt, -1);
		return 1;
	}
	lock_mount_hash();
	if (unlikely(bastard->mnt_flags & MNT_DOOMED)) {
		mnt_add_count(mnt, -1);
		unlock_mount_hash();
		return 1;
	}
	unlock_mount_hash();
	/* caller will mntput() */
	return -1;
}