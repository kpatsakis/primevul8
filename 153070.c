struct posix_acl *get_cached_acl(struct inode *inode, int type)
{
	struct posix_acl **p = acl_by_type(inode, type);
	struct posix_acl *acl;

	for (;;) {
		rcu_read_lock();
		acl = rcu_dereference(*p);
		if (!acl || is_uncached_acl(acl) ||
		    atomic_inc_not_zero(&acl->a_refcount))
			break;
		rcu_read_unlock();
		cpu_relax();
	}
	rcu_read_unlock();
	return acl;
}