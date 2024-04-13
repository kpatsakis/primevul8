static loff_t nfs42_remap_file_range(struct file *src_file, loff_t src_off,
		struct file *dst_file, loff_t dst_off, loff_t count,
		unsigned int remap_flags)
{
	struct inode *dst_inode = file_inode(dst_file);
	struct nfs_server *server = NFS_SERVER(dst_inode);
	struct inode *src_inode = file_inode(src_file);
	unsigned int bs = server->clone_blksize;
	bool same_inode = false;
	int ret;

	/* NFS does not support deduplication. */
	if (remap_flags & REMAP_FILE_DEDUP)
		return -EOPNOTSUPP;

	if (remap_flags & ~REMAP_FILE_ADVISORY)
		return -EINVAL;

	if (IS_SWAPFILE(dst_inode) || IS_SWAPFILE(src_inode))
		return -ETXTBSY;

	/* check alignment w.r.t. clone_blksize */
	ret = -EINVAL;
	if (bs) {
		if (!IS_ALIGNED(src_off, bs) || !IS_ALIGNED(dst_off, bs))
			goto out;
		if (!IS_ALIGNED(count, bs) && i_size_read(src_inode) != (src_off + count))
			goto out;
	}

	if (src_inode == dst_inode)
		same_inode = true;

	/* XXX: do we lock at all? what if server needs CB_RECALL_LAYOUT? */
	if (same_inode) {
		inode_lock(src_inode);
	} else if (dst_inode < src_inode) {
		inode_lock_nested(dst_inode, I_MUTEX_PARENT);
		inode_lock_nested(src_inode, I_MUTEX_CHILD);
	} else {
		inode_lock_nested(src_inode, I_MUTEX_PARENT);
		inode_lock_nested(dst_inode, I_MUTEX_CHILD);
	}

	/* flush all pending writes on both src and dst so that server
	 * has the latest data */
	ret = nfs_sync_inode(src_inode);
	if (ret)
		goto out_unlock;
	ret = nfs_sync_inode(dst_inode);
	if (ret)
		goto out_unlock;

	ret = nfs42_proc_clone(src_file, dst_file, src_off, dst_off, count);

	/* truncate inode page cache of the dst range so that future reads can fetch
	 * new data from server */
	if (!ret)
		truncate_inode_pages_range(&dst_inode->i_data, dst_off, dst_off + count - 1);

out_unlock:
	if (same_inode) {
		inode_unlock(src_inode);
	} else if (dst_inode < src_inode) {
		inode_unlock(src_inode);
		inode_unlock(dst_inode);
	} else {
		inode_unlock(dst_inode);
		inode_unlock(src_inode);
	}
out:
	return ret < 0 ? ret : count;
}