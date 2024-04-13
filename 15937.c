static bool check_for_nsfs_mounts(struct mount *subtree)
{
	struct mount *p;
	bool ret = false;

	lock_mount_hash();
	for (p = subtree; p; p = next_mnt(p, subtree))
		if (mnt_ns_loop(p->mnt.mnt_root))
			goto out;

	ret = true;
out:
	unlock_mount_hash();
	return ret;
}