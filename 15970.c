static void __init init_mount_tree(void)
{
	struct vfsmount *mnt;
	struct mount *m;
	struct mnt_namespace *ns;
	struct path root;

	mnt = vfs_kern_mount(&rootfs_fs_type, 0, "rootfs", NULL);
	if (IS_ERR(mnt))
		panic("Can't create rootfs");

	ns = alloc_mnt_ns(&init_user_ns, false);
	if (IS_ERR(ns))
		panic("Can't allocate initial namespace");
	m = real_mount(mnt);
	m->mnt_ns = ns;
	ns->root = m;
	ns->mounts = 1;
	list_add(&m->mnt_list, &ns->list);
	init_task.nsproxy->mnt_ns = ns;
	get_mnt_ns(ns);

	root.mnt = mnt;
	root.dentry = mnt->mnt_root;
	mnt->mnt_flags |= MNT_LOCKED;

	set_fs_pwd(current->fs, &root);
	set_fs_root(current->fs, &root);
}