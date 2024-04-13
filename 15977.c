static struct mount *mnt_list_next(struct mnt_namespace *ns,
				   struct list_head *p)
{
	struct mount *mnt, *ret = NULL;

	lock_ns_list(ns);
	list_for_each_continue(p, &ns->list) {
		mnt = list_entry(p, typeof(*mnt), mnt_list);
		if (!mnt_is_cursor(mnt)) {
			ret = mnt;
			break;
		}
	}
	unlock_ns_list(ns);

	return ret;
}