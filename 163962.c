xfs_attr_fork_remove(
	struct xfs_inode	*ip,
	struct xfs_trans	*tp)
{
	ASSERT(ip->i_afp->if_nextents == 0);

	xfs_idestroy_fork(ip->i_afp);
	kmem_cache_free(xfs_ifork_zone, ip->i_afp);
	ip->i_afp = NULL;
	ip->i_d.di_forkoff = 0;
	xfs_trans_log_inode(tp, ip, XFS_ILOG_CORE);
}