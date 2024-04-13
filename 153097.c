void forget_cached_acl(struct inode *inode, int type)
{
	__forget_cached_acl(acl_by_type(inode, type));
}