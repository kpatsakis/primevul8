static struct file *open_detached_copy(struct path *path, bool recursive)
{
	struct user_namespace *user_ns = current->nsproxy->mnt_ns->user_ns;
	struct mnt_namespace *ns = alloc_mnt_ns(user_ns, true);
	struct mount *mnt, *p;
	struct file *file;

	if (IS_ERR(ns))
		return ERR_CAST(ns);

	namespace_lock();
	mnt = __do_loopback(path, recursive);
	if (IS_ERR(mnt)) {
		namespace_unlock();
		free_mnt_ns(ns);
		return ERR_CAST(mnt);
	}

	lock_mount_hash();
	for (p = mnt; p; p = next_mnt(p, mnt)) {
		p->mnt_ns = ns;
		ns->mounts++;
	}
	ns->root = mnt;
	list_add_tail(&ns->list, &mnt->mnt_list);
	mntget(&mnt->mnt);
	unlock_mount_hash();
	namespace_unlock();

	mntput(path->mnt);
	path->mnt = &mnt->mnt;
	file = dentry_open(path, O_PATH, current_cred());
	if (IS_ERR(file))
		dissolve_on_fput(path->mnt);
	else
		file->f_mode |= FMODE_NEED_UNMOUNT;
	return file;
}