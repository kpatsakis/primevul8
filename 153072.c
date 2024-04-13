static void __forget_cached_acl(struct posix_acl **p)
{
	struct posix_acl *old;

	old = xchg(p, ACL_NOT_CACHED);
	if (!is_uncached_acl(old))
		posix_acl_release(old);
}