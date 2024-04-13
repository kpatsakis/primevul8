void put_mnt_ns(struct mnt_namespace *ns)
{
	if (!refcount_dec_and_test(&ns->ns.count))
		return;
	drop_collected_mounts(&ns->root->mnt);
	free_mnt_ns(ns);
}