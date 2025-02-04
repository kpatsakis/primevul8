static void put_mountpoint(struct mountpoint *mp)
{
	if (!--mp->m_count) {
		struct dentry *dentry = mp->m_dentry;
		spin_lock(&dentry->d_lock);
		dentry->d_flags &= ~DCACHE_MOUNTED;
		spin_unlock(&dentry->d_lock);
		hlist_del(&mp->m_hash);
		kfree(mp);
	}
}