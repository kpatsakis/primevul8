posix_acl_clone(const struct posix_acl *acl, gfp_t flags)
{
	struct posix_acl *clone = NULL;

	if (acl) {
		int size = sizeof(struct posix_acl) + acl->a_count *
		           sizeof(struct posix_acl_entry);
		clone = kmemdup(acl, size, flags);
		if (clone)
			atomic_set(&clone->a_refcount, 1);
	}
	return clone;
}