static void *proc_self_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	struct pid_namespace *ns = dentry->d_sb->s_fs_info;
	pid_t tgid = task_tgid_nr_ns(current, ns);
	char *name = ERR_PTR(-ENOENT);
	if (tgid) {
		name = __getname();
		if (!name)
			name = ERR_PTR(-ENOMEM);
		else
			sprintf(name, "%d", tgid);
	}
	nd_set_link(nd, name);
	return NULL;
}