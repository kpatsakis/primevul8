posix_acl_alloc(int count, gfp_t flags)
{
	const size_t size = sizeof(struct posix_acl) +
	                    count * sizeof(struct posix_acl_entry);
	struct posix_acl *acl = kmalloc(size, flags);
	if (acl)
		posix_acl_init(acl, count);
	return acl;
}