static void namespace_unlock(void)
{
	struct hlist_head head;
	struct hlist_node *p;
	struct mount *m;
	LIST_HEAD(list);

	hlist_move_list(&unmounted, &head);
	list_splice_init(&ex_mountpoints, &list);

	up_write(&namespace_sem);

	shrink_dentry_list(&list);

	if (likely(hlist_empty(&head)))
		return;

	synchronize_rcu_expedited();

	hlist_for_each_entry_safe(m, p, &head, mnt_umount) {
		hlist_del(&m->mnt_umount);
		mntput(&m->mnt);
	}
}