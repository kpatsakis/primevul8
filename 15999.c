void mnt_cursor_del(struct mnt_namespace *ns, struct mount *cursor)
{
	down_read(&namespace_sem);
	lock_ns_list(ns);
	list_del(&cursor->mnt_list);
	unlock_ns_list(ns);
	up_read(&namespace_sem);
}