struct posix_acl *get_acl(struct inode *inode, int type)
{
	void *sentinel;
	struct posix_acl **p;
	struct posix_acl *acl;

	/*
	 * The sentinel is used to detect when another operation like
	 * set_cached_acl() or forget_cached_acl() races with get_acl().
	 * It is guaranteed that is_uncached_acl(sentinel) is true.
	 */

	acl = get_cached_acl(inode, type);
	if (!is_uncached_acl(acl))
		return acl;

	if (!IS_POSIXACL(inode))
		return NULL;

	sentinel = uncached_acl_sentinel(current);
	p = acl_by_type(inode, type);

	/*
	 * If the ACL isn't being read yet, set our sentinel.  Otherwise, the
	 * current value of the ACL will not be ACL_NOT_CACHED and so our own
	 * sentinel will not be set; another task will update the cache.  We
	 * could wait for that other task to complete its job, but it's easier
	 * to just call ->get_acl to fetch the ACL ourself.  (This is going to
	 * be an unlikely race.)
	 */
	if (cmpxchg(p, ACL_NOT_CACHED, sentinel) != ACL_NOT_CACHED)
		/* fall through */ ;

	/*
	 * Normally, the ACL returned by ->get_acl will be cached.
	 * A filesystem can prevent that by calling
	 * forget_cached_acl(inode, type) in ->get_acl.
	 *
	 * If the filesystem doesn't have a get_acl() function at all, we'll
	 * just create the negative cache entry.
	 */
	if (!inode->i_op->get_acl) {
		set_cached_acl(inode, type, NULL);
		return NULL;
	}
	acl = inode->i_op->get_acl(inode, type);

	if (IS_ERR(acl)) {
		/*
		 * Remove our sentinel so that we don't block future attempts
		 * to cache the ACL.
		 */
		cmpxchg(p, sentinel, ACL_NOT_CACHED);
		return acl;
	}

	/*
	 * Cache the result, but only if our sentinel is still in place.
	 */
	posix_acl_dup(acl);
	if (unlikely(cmpxchg(p, sentinel, acl) != sentinel))
		posix_acl_release(acl);
	return acl;
}