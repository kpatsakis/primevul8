posix_acl_chmod(struct inode *inode, umode_t mode)
{
	struct posix_acl *acl;
	int ret = 0;

	if (!IS_POSIXACL(inode))
		return 0;
	if (!inode->i_op->set_acl)
		return -EOPNOTSUPP;

	acl = get_acl(inode, ACL_TYPE_ACCESS);
	if (IS_ERR_OR_NULL(acl)) {
		if (acl == ERR_PTR(-EOPNOTSUPP))
			return 0;
		return PTR_ERR(acl);
	}

	ret = __posix_acl_chmod(&acl, GFP_KERNEL, mode);
	if (ret)
		return ret;
	ret = inode->i_op->set_acl(inode, acl, ACL_TYPE_ACCESS);
	posix_acl_release(acl);
	return ret;
}