int simple_set_acl(struct inode *inode, struct posix_acl *acl, int type)
{
	int error;

	if (type == ACL_TYPE_ACCESS) {
		error = posix_acl_equiv_mode(acl, &inode->i_mode);
		if (error < 0)
			return 0;
		if (error == 0)
			acl = NULL;
	}

	inode->i_ctime = CURRENT_TIME;
	set_cached_acl(inode, type, acl);
	return 0;
}