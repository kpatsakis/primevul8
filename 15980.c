static int mntns_install(struct nsset *nsset, struct ns_common *ns)
{
	struct nsproxy *nsproxy = nsset->nsproxy;
	struct fs_struct *fs = nsset->fs;
	struct mnt_namespace *mnt_ns = to_mnt_ns(ns), *old_mnt_ns;
	struct user_namespace *user_ns = nsset->cred->user_ns;
	struct path root;
	int err;

	if (!ns_capable(mnt_ns->user_ns, CAP_SYS_ADMIN) ||
	    !ns_capable(user_ns, CAP_SYS_CHROOT) ||
	    !ns_capable(user_ns, CAP_SYS_ADMIN))
		return -EPERM;

	if (is_anon_ns(mnt_ns))
		return -EINVAL;

	if (fs->users != 1)
		return -EINVAL;

	get_mnt_ns(mnt_ns);
	old_mnt_ns = nsproxy->mnt_ns;
	nsproxy->mnt_ns = mnt_ns;

	/* Find the root */
	err = vfs_path_lookup(mnt_ns->root->mnt.mnt_root, &mnt_ns->root->mnt,
				"/", LOOKUP_DOWN, &root);
	if (err) {
		/* revert to old namespace */
		nsproxy->mnt_ns = old_mnt_ns;
		put_mnt_ns(mnt_ns);
		return err;
	}

	put_mnt_ns(old_mnt_ns);

	/* Update the pwd and root */
	set_fs_pwd(fs, &root);
	set_fs_root(fs, &root);

	path_put(&root);
	return 0;
}