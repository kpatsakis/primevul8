static bool disconnect_mount(struct mount *mnt, enum umount_tree_flags how)
{
	/* Leaving mounts connected is only valid for lazy umounts */
	if (how & UMOUNT_SYNC)
		return true;

	/* A mount without a parent has nothing to be connected to */
	if (!mnt_has_parent(mnt))
		return true;

	/* Because the reference counting rules change when mounts are
	 * unmounted and connected, umounted mounts may not be
	 * connected to mounted mounts.
	 */
	if (!(mnt->mnt_parent->mnt.mnt_flags & MNT_UMOUNT))
		return true;

	/* Has it been requested that the mount remain connected? */
	if (how & UMOUNT_CONNECTED)
		return false;

	/* Is the mount locked such that it needs to remain connected? */
	if (IS_MNT_LOCKED(mnt))
		return false;

	/* By default disconnect the mount */
	return true;
}