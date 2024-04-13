static int selinux_inode_getattr(struct vfsmount *mnt, struct dentry *dentry)
{
	const struct cred *cred = current_cred();
	struct path path;

	path.dentry = dentry;
	path.mnt = mnt;

	return path_has_perm(cred, &path, FILE__GETATTR);
}