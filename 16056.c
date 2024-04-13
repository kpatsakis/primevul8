static inline void lock_ns_list(struct mnt_namespace *ns)
{
	spin_lock(&ns->ns_lock);
}