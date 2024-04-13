void set_cached_acl(struct inode *inode, int type, struct posix_acl *acl)
{
	struct posix_acl **p = acl_by_type(inode, type);
	struct posix_acl *old;

	old = xchg(p, posix_acl_dup(acl));
	if (!is_uncached_acl(old))
		posix_acl_release(old);
}