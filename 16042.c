bool path_is_mountpoint(const struct path *path)
{
	unsigned seq;
	bool res;

	if (!d_mountpoint(path->dentry))
		return false;

	rcu_read_lock();
	do {
		seq = read_seqbegin(&mount_lock);
		res = __path_is_mountpoint(path);
	} while (read_seqretry(&mount_lock, seq));
	rcu_read_unlock();

	return res;
}