xfs_alloc_set_aside(
	struct xfs_mount	*mp)
{
	return mp->m_sb.sb_agcount * (XFS_ALLOC_AGFL_RESERVE + 4);
}