static void cleanup_mnt(struct mount *mnt)
{
	struct hlist_node *p;
	struct mount *m;
	/*
	 * The warning here probably indicates that somebody messed
	 * up a mnt_want/drop_write() pair.  If this happens, the
	 * filesystem was probably unable to make r/w->r/o transitions.
	 * The locking used to deal with mnt_count decrement provides barriers,
	 * so mnt_get_writers() below is safe.
	 */
	WARN_ON(mnt_get_writers(mnt));
	if (unlikely(mnt->mnt_pins.first))
		mnt_pin_kill(mnt);
	hlist_for_each_entry_safe(m, p, &mnt->mnt_stuck_children, mnt_umount) {
		hlist_del(&m->mnt_umount);
		mntput(&m->mnt);
	}
	fsnotify_vfsmount_delete(&mnt->mnt);
	dput(mnt->mnt.mnt_root);
	deactivate_super(mnt->mnt.mnt_sb);
	mnt_free_id(mnt);
	call_rcu(&mnt->mnt_rcu, delayed_free_vfsmnt);
}