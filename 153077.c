posix_acl_xattr_list(struct dentry *dentry)
{
	return IS_POSIXACL(d_backing_inode(dentry));
}