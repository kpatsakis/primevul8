static bool mnt_already_visible(struct mnt_namespace *ns,
				const struct super_block *sb,
				int *new_mnt_flags)
{
	int new_flags = *new_mnt_flags;
	struct mount *mnt;
	bool visible = false;

	down_read(&namespace_sem);
	lock_ns_list(ns);
	list_for_each_entry(mnt, &ns->list, mnt_list) {
		struct mount *child;
		int mnt_flags;

		if (mnt_is_cursor(mnt))
			continue;

		if (mnt->mnt.mnt_sb->s_type != sb->s_type)
			continue;

		/* This mount is not fully visible if it's root directory
		 * is not the root directory of the filesystem.
		 */
		if (mnt->mnt.mnt_root != mnt->mnt.mnt_sb->s_root)
			continue;

		/* A local view of the mount flags */
		mnt_flags = mnt->mnt.mnt_flags;

		/* Don't miss readonly hidden in the superblock flags */
		if (sb_rdonly(mnt->mnt.mnt_sb))
			mnt_flags |= MNT_LOCK_READONLY;

		/* Verify the mount flags are equal to or more permissive
		 * than the proposed new mount.
		 */
		if ((mnt_flags & MNT_LOCK_READONLY) &&
		    !(new_flags & MNT_READONLY))
			continue;
		if ((mnt_flags & MNT_LOCK_ATIME) &&
		    ((mnt_flags & MNT_ATIME_MASK) != (new_flags & MNT_ATIME_MASK)))
			continue;

		/* This mount is not fully visible if there are any
		 * locked child mounts that cover anything except for
		 * empty directories.
		 */
		list_for_each_entry(child, &mnt->mnt_mounts, mnt_child) {
			struct inode *inode = child->mnt_mountpoint->d_inode;
			/* Only worry about locked mounts */
			if (!(child->mnt.mnt_flags & MNT_LOCKED))
				continue;
			/* Is the directory permanetly empty? */
			if (!is_empty_dir_inode(inode))
				goto next;
		}
		/* Preserve the locked attributes */
		*new_mnt_flags |= mnt_flags & (MNT_LOCK_READONLY | \
					       MNT_LOCK_ATIME);
		visible = true;
		goto found;
	next:	;
	}
found:
	unlock_ns_list(ns);
	up_read(&namespace_sem);
	return visible;
}