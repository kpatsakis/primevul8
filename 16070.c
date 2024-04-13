static void free_mnt_ns(struct mnt_namespace *ns)
{
	if (!is_anon_ns(ns))
		ns_free_inum(&ns->ns);
	dec_mnt_namespaces(ns->ucounts);
	put_user_ns(ns->user_ns);
	kfree(ns);
}