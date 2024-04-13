posix_acl_init(struct posix_acl *acl, int count)
{
	atomic_set(&acl->a_refcount, 1);
	acl->a_count = count;
}