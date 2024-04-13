static void commit_tree(struct mount *mnt)
{
	struct mount *parent = mnt->mnt_parent;
	struct mount *m;
	LIST_HEAD(head);
	struct mnt_namespace *n = parent->mnt_ns;

	BUG_ON(parent == mnt);

	list_add_tail(&head, &mnt->mnt_list);
	list_for_each_entry(m, &head, mnt_list)
		m->mnt_ns = n;

	list_splice(&head, n->list.prev);

	n->mounts += n->pending_mounts;
	n->pending_mounts = 0;

	__attach_mnt(mnt, parent);
	touch_mnt_namespace(n);
}