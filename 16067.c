struct vfsmount *collect_mounts(const struct path *path)
{
	struct mount *tree;
	namespace_lock();
	if (!check_mnt(real_mount(path->mnt)))
		tree = ERR_PTR(-EINVAL);
	else
		tree = copy_tree(real_mount(path->mnt), path->dentry,
				 CL_COPY_ALL | CL_PRIVATE);
	namespace_unlock();
	if (IS_ERR(tree))
		return ERR_CAST(tree);
	return &tree->mnt;
}