posix_acl_from_mode(umode_t mode, gfp_t flags)
{
	struct posix_acl *acl = posix_acl_alloc(3, flags);
	if (!acl)
		return ERR_PTR(-ENOMEM);

	acl->a_entries[0].e_tag  = ACL_USER_OBJ;
	acl->a_entries[0].e_perm = (mode & S_IRWXU) >> 6;

	acl->a_entries[1].e_tag  = ACL_GROUP_OBJ;
	acl->a_entries[1].e_perm = (mode & S_IRWXG) >> 3;

	acl->a_entries[2].e_tag  = ACL_OTHER;
	acl->a_entries[2].e_perm = (mode & S_IRWXO);
	return acl;
}