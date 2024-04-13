xfs_attr_leaf_lasthash(
	struct xfs_buf	*bp,
	int		*count)
{
	struct xfs_attr3_icleaf_hdr ichdr;
	struct xfs_attr_leaf_entry *entries;
	struct xfs_mount *mp = bp->b_mount;

	xfs_attr3_leaf_hdr_from_disk(mp->m_attr_geo, &ichdr, bp->b_addr);
	entries = xfs_attr3_leaf_entryp(bp->b_addr);
	if (count)
		*count = ichdr.count;
	if (!ichdr.count)
		return 0;
	return be32_to_cpu(entries[ichdr.count - 1].hashval);
}