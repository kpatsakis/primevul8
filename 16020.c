static void delayed_mntput(struct work_struct *unused)
{
	struct llist_node *node = llist_del_all(&delayed_mntput_list);
	struct mount *m, *t;

	llist_for_each_entry_safe(m, t, node, mnt_llist)
		cleanup_mnt(m);
}