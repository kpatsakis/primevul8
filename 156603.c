xfs_agfl_size(
	struct xfs_mount	*mp)
{
	unsigned int		size = mp->m_sb.sb_sectsize;

	if (xfs_sb_version_hascrc(&mp->m_sb))
		size -= sizeof(struct xfs_agfl);

	return size / sizeof(xfs_agblock_t);
}