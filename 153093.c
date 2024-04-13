posix_acl_xattr_set(const struct xattr_handler *handler,
		    struct dentry *unused, struct inode *inode,
		    const char *name, const void *value,
		    size_t size, int flags)
{
	struct posix_acl *acl = NULL;
	int ret;

	if (value) {
		acl = posix_acl_from_xattr(&init_user_ns, value, size);
		if (IS_ERR(acl))
			return PTR_ERR(acl);
	}
	ret = set_posix_acl(inode, handler->flags, acl);
	posix_acl_release(acl);
	return ret;
}