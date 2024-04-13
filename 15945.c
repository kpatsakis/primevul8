static int attach_recursive_mnt(struct mount *source_mnt,
			struct mount *dest_mnt,
			struct mountpoint *dest_mp,
			bool moving)
{
	struct user_namespace *user_ns = current->nsproxy->mnt_ns->user_ns;
	HLIST_HEAD(tree_list);
	struct mnt_namespace *ns = dest_mnt->mnt_ns;
	struct mountpoint *smp;
	struct mount *child, *p;
	struct hlist_node *n;
	int err;

	/* Preallocate a mountpoint in case the new mounts need
	 * to be tucked under other mounts.
	 */
	smp = get_mountpoint(source_mnt->mnt.mnt_root);
	if (IS_ERR(smp))
		return PTR_ERR(smp);

	/* Is there space to add these mounts to the mount namespace? */
	if (!moving) {
		err = count_mounts(ns, source_mnt);
		if (err)
			goto out;
	}

	if (IS_MNT_SHARED(dest_mnt)) {
		err = invent_group_ids(source_mnt, true);
		if (err)
			goto out;
		err = propagate_mnt(dest_mnt, dest_mp, source_mnt, &tree_list);
		lock_mount_hash();
		if (err)
			goto out_cleanup_ids;
		for (p = source_mnt; p; p = next_mnt(p, source_mnt))
			set_mnt_shared(p);
	} else {
		lock_mount_hash();
	}
	if (moving) {
		unhash_mnt(source_mnt);
		attach_mnt(source_mnt, dest_mnt, dest_mp);
		touch_mnt_namespace(source_mnt->mnt_ns);
	} else {
		if (source_mnt->mnt_ns) {
			/* move from anon - the caller will destroy */
			list_del_init(&source_mnt->mnt_ns->list);
		}
		mnt_set_mountpoint(dest_mnt, dest_mp, source_mnt);
		commit_tree(source_mnt);
	}

	hlist_for_each_entry_safe(child, n, &tree_list, mnt_hash) {
		struct mount *q;
		hlist_del_init(&child->mnt_hash);
		q = __lookup_mnt(&child->mnt_parent->mnt,
				 child->mnt_mountpoint);
		if (q)
			mnt_change_mountpoint(child, smp, q);
		/* Notice when we are propagating across user namespaces */
		if (child->mnt_parent->mnt_ns->user_ns != user_ns)
			lock_mnt_tree(child);
		child->mnt.mnt_flags &= ~MNT_LOCKED;
		commit_tree(child);
	}
	put_mountpoint(smp);
	unlock_mount_hash();

	return 0;

 out_cleanup_ids:
	while (!hlist_empty(&tree_list)) {
		child = hlist_entry(tree_list.first, struct mount, mnt_hash);
		child->mnt_parent->mnt_ns->pending_mounts = 0;
		umount_tree(child, UMOUNT_SYNC);
	}
	unlock_mount_hash();
	cleanup_group_ids(source_mnt, NULL);
 out:
	ns->pending_mounts = 0;

	read_seqlock_excl(&mount_lock);
	put_mountpoint(smp);
	read_sequnlock_excl(&mount_lock);

	return err;
}