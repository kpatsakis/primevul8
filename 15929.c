bool __is_local_mountpoint(struct dentry *dentry)
{
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	struct mount *mnt;
	bool is_covered = false;

	down_read(&namespace_sem);
	lock_ns_list(ns);
	list_for_each_entry(mnt, &ns->list, mnt_list) {
		if (mnt_is_cursor(mnt))
			continue;
		is_covered = (mnt->mnt_mountpoint == dentry);
		if (is_covered)
			break;
	}
	unlock_ns_list(ns);
	up_read(&namespace_sem);

	return is_covered;
}