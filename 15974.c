static int do_mount_setattr(struct path *path, struct mount_kattr *kattr)
{
	struct mount *mnt = real_mount(path->mnt), *last = NULL;
	int err = 0;

	if (path->dentry != mnt->mnt.mnt_root)
		return -EINVAL;

	if (kattr->propagation) {
		/*
		 * Only take namespace_lock() if we're actually changing
		 * propagation.
		 */
		namespace_lock();
		if (kattr->propagation == MS_SHARED) {
			err = invent_group_ids(mnt, kattr->recurse);
			if (err) {
				namespace_unlock();
				return err;
			}
		}
	}

	lock_mount_hash();

	/*
	 * Get the mount tree in a shape where we can change mount
	 * properties without failure.
	 */
	last = mount_setattr_prepare(kattr, mnt, &err);
	if (last) /* Commit all changes or revert to the old state. */
		mount_setattr_commit(kattr, mnt, last, err);

	unlock_mount_hash();

	if (kattr->propagation) {
		namespace_unlock();
		if (err)
			cleanup_group_ids(mnt, NULL);
	}

	return err;
}