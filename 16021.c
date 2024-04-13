static struct user_namespace *mntns_owner(struct ns_common *ns)
{
	return to_mnt_ns(ns)->user_ns;
}