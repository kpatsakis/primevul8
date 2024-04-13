static void mount_setattr_commit(struct mount_kattr *kattr,
				 struct mount *mnt, struct mount *last,
				 int err)
{
	struct mount *m = mnt;

	do {
		if (!err) {
			unsigned int flags;

			do_idmap_mount(kattr, m);
			flags = recalc_flags(kattr, m);
			WRITE_ONCE(m->mnt.mnt_flags, flags);
		}

		/*
		 * We either set MNT_READONLY above so make it visible
		 * before ~MNT_WRITE_HOLD or we failed to recursively
		 * apply mount options.
		 */
		if ((kattr->attr_set & MNT_READONLY) &&
		    (m->mnt.mnt_flags & MNT_WRITE_HOLD))
			mnt_unhold_writers(m);

		if (!err && kattr->propagation)
			change_mnt_propagation(m, kattr->propagation);

		/*
		 * On failure, only cleanup until we found the first mount
		 * we failed to handle.
		 */
		if (err && m == last)
			break;
	} while (kattr->recurse && (m = next_mnt(m, mnt)));

	if (!err)
		touch_mnt_namespace(mnt->mnt_ns);
}