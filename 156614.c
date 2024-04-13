xfs_alloc_min_freelist(
	struct xfs_mount	*mp,
	struct xfs_perag	*pag)
{
	/* AG btrees have at least 1 level. */
	static const uint8_t	fake_levels[XFS_BTNUM_AGF] = {1, 1, 1};
	const uint8_t		*levels = pag ? pag->pagf_levels : fake_levels;
	unsigned int		min_free;

	ASSERT(mp->m_ag_maxlevels > 0);

	/* space needed by-bno freespace btree */
	min_free = min_t(unsigned int, levels[XFS_BTNUM_BNOi] + 1,
				       mp->m_ag_maxlevels);
	/* space needed by-size freespace btree */
	min_free += min_t(unsigned int, levels[XFS_BTNUM_CNTi] + 1,
				       mp->m_ag_maxlevels);
	/* space needed reverse mapping used space btree */
	if (xfs_sb_version_hasrmapbt(&mp->m_sb))
		min_free += min_t(unsigned int, levels[XFS_BTNUM_RMAPi] + 1,
						mp->m_rmap_maxlevels);

	return min_free;
}