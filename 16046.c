static int build_mount_kattr(const struct mount_attr *attr, size_t usize,
			     struct mount_kattr *kattr, unsigned int flags)
{
	unsigned int lookup_flags = LOOKUP_AUTOMOUNT | LOOKUP_FOLLOW;

	if (flags & AT_NO_AUTOMOUNT)
		lookup_flags &= ~LOOKUP_AUTOMOUNT;
	if (flags & AT_SYMLINK_NOFOLLOW)
		lookup_flags &= ~LOOKUP_FOLLOW;
	if (flags & AT_EMPTY_PATH)
		lookup_flags |= LOOKUP_EMPTY;

	*kattr = (struct mount_kattr) {
		.lookup_flags	= lookup_flags,
		.recurse	= !!(flags & AT_RECURSIVE),
	};

	if (attr->propagation & ~MOUNT_SETATTR_PROPAGATION_FLAGS)
		return -EINVAL;
	if (hweight32(attr->propagation & MOUNT_SETATTR_PROPAGATION_FLAGS) > 1)
		return -EINVAL;
	kattr->propagation = attr->propagation;

	if ((attr->attr_set | attr->attr_clr) & ~MOUNT_SETATTR_VALID_FLAGS)
		return -EINVAL;

	kattr->attr_set = attr_flags_to_mnt_flags(attr->attr_set);
	kattr->attr_clr = attr_flags_to_mnt_flags(attr->attr_clr);

	/*
	 * Since the MOUNT_ATTR_<atime> values are an enum, not a bitmap,
	 * users wanting to transition to a different atime setting cannot
	 * simply specify the atime setting in @attr_set, but must also
	 * specify MOUNT_ATTR__ATIME in the @attr_clr field.
	 * So ensure that MOUNT_ATTR__ATIME can't be partially set in
	 * @attr_clr and that @attr_set can't have any atime bits set if
	 * MOUNT_ATTR__ATIME isn't set in @attr_clr.
	 */
	if (attr->attr_clr & MOUNT_ATTR__ATIME) {
		if ((attr->attr_clr & MOUNT_ATTR__ATIME) != MOUNT_ATTR__ATIME)
			return -EINVAL;

		/*
		 * Clear all previous time settings as they are mutually
		 * exclusive.
		 */
		kattr->attr_clr |= MNT_RELATIME | MNT_NOATIME;
		switch (attr->attr_set & MOUNT_ATTR__ATIME) {
		case MOUNT_ATTR_RELATIME:
			kattr->attr_set |= MNT_RELATIME;
			break;
		case MOUNT_ATTR_NOATIME:
			kattr->attr_set |= MNT_NOATIME;
			break;
		case MOUNT_ATTR_STRICTATIME:
			break;
		default:
			return -EINVAL;
		}
	} else {
		if (attr->attr_set & MOUNT_ATTR__ATIME)
			return -EINVAL;
	}

	return build_mount_idmapped(attr, usize, kattr, flags);
}