xfs_alloc_compute_maxlevels(
	xfs_mount_t	*mp)	/* file system mount structure */
{
	mp->m_ag_maxlevels = xfs_btree_compute_maxlevels(mp->m_alloc_mnr,
			(mp->m_sb.sb_agblocks + 1) / 2);
}