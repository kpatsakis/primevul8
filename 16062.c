static struct mount *mount_setattr_prepare(struct mount_kattr *kattr,
					   struct mount *mnt, int *err)
{
	struct mount *m = mnt, *last = NULL;

	if (!is_mounted(&m->mnt)) {
		*err = -EINVAL;
		goto out;
	}

	if (!(mnt_has_parent(m) ? check_mnt(m) : is_anon_ns(m->mnt_ns))) {
		*err = -EINVAL;
		goto out;
	}

	do {
		unsigned int flags;

		flags = recalc_flags(kattr, m);
		if (!can_change_locked_flags(m, flags)) {
			*err = -EPERM;
			goto out;
		}

		*err = can_idmap_mount(kattr, m);
		if (*err)
			goto out;

		last = m;

		if ((kattr->attr_set & MNT_READONLY) &&
		    !(m->mnt.mnt_flags & MNT_READONLY)) {
			*err = mnt_hold_writers(m);
			if (*err)
				goto out;
		}
	} while (kattr->recurse && (m = next_mnt(m, mnt)));

out:
	return last;
}