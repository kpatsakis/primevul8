static void unlock_mount(struct mountpoint *where)
{
	struct dentry *dentry = where->m_dentry;

	read_seqlock_excl(&mount_lock);
	put_mountpoint(where);
	read_sequnlock_excl(&mount_lock);

	namespace_unlock();
	inode_unlock(dentry->d_inode);
}