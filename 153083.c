__posix_acl_chmod(struct posix_acl **acl, gfp_t gfp, umode_t mode)
{
	struct posix_acl *clone = posix_acl_clone(*acl, gfp);
	int err = -ENOMEM;
	if (clone) {
		err = __posix_acl_chmod_masq(clone, mode);
		if (err) {
			posix_acl_release(clone);
			clone = NULL;
		}
	}
	posix_acl_release(*acl);
	*acl = clone;
	return err;
}