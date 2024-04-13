static int selinux_inode_follow_link(struct dentry *dentry, struct nameidata *nameidata)
{
	const struct cred *cred = current_cred();

	return dentry_has_perm(cred, dentry, FILE__READ);
}