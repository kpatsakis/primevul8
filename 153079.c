set_posix_acl(struct inode *inode, int type, struct posix_acl *acl)
{
	if (!IS_POSIXACL(inode))
		return -EOPNOTSUPP;
	if (!inode->i_op->set_acl)
		return -EOPNOTSUPP;

	if (type == ACL_TYPE_DEFAULT && !S_ISDIR(inode->i_mode))
		return acl ? -EACCES : 0;
	if (!inode_owner_or_capable(inode))
		return -EPERM;

	if (acl) {
		int ret = posix_acl_valid(acl);
		if (ret)
			return ret;
	}
	return inode->i_op->set_acl(inode, acl, type);
}