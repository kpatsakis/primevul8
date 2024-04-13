static int build_mount_idmapped(const struct mount_attr *attr, size_t usize,
				struct mount_kattr *kattr, unsigned int flags)
{
	int err = 0;
	struct ns_common *ns;
	struct user_namespace *mnt_userns;
	struct file *file;

	if (!((attr->attr_set | attr->attr_clr) & MOUNT_ATTR_IDMAP))
		return 0;

	/*
	 * We currently do not support clearing an idmapped mount. If this ever
	 * is a use-case we can revisit this but for now let's keep it simple
	 * and not allow it.
	 */
	if (attr->attr_clr & MOUNT_ATTR_IDMAP)
		return -EINVAL;

	if (attr->userns_fd > INT_MAX)
		return -EINVAL;

	file = fget(attr->userns_fd);
	if (!file)
		return -EBADF;

	if (!proc_ns_file(file)) {
		err = -EINVAL;
		goto out_fput;
	}

	ns = get_proc_ns(file_inode(file));
	if (ns->ops->type != CLONE_NEWUSER) {
		err = -EINVAL;
		goto out_fput;
	}

	/*
	 * The init_user_ns is used to indicate that a vfsmount is not idmapped.
	 * This is simpler than just having to treat NULL as unmapped. Users
	 * wanting to idmap a mount to init_user_ns can just use a namespace
	 * with an identity mapping.
	 */
	mnt_userns = container_of(ns, struct user_namespace, ns);
	if (mnt_userns == &init_user_ns) {
		err = -EPERM;
		goto out_fput;
	}
	kattr->mnt_userns = get_user_ns(mnt_userns);

out_fput:
	fput(file);
	return err;
}