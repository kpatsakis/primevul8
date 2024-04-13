static inline void unlock_ns_list(struct mnt_namespace *ns)
{
	spin_unlock(&ns->ns_lock);
}