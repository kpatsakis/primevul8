bool path_is_under(const struct path *path1, const struct path *path2)
{
	bool res;
	read_seqlock_excl(&mount_lock);
	res = is_path_reachable(real_mount(path1->mnt), path1->dentry, path2);
	read_sequnlock_excl(&mount_lock);
	return res;
}