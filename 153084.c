void forget_all_cached_acls(struct inode *inode)
{
	__forget_cached_acl(&inode->i_acl);
	__forget_cached_acl(&inode->i_default_acl);
}