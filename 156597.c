xfs_refc_block(
	struct xfs_mount	*mp)
{
	if (xfs_sb_version_hasrmapbt(&mp->m_sb))
		return XFS_RMAP_BLOCK(mp) + 1;
	if (xfs_sb_version_hasfinobt(&mp->m_sb))
		return XFS_FIBT_BLOCK(mp) + 1;
	return XFS_IBT_BLOCK(mp) + 1;
}