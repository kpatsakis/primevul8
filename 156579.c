xfs_alloc_has_record(
	struct xfs_btree_cur	*cur,
	xfs_agblock_t		bno,
	xfs_extlen_t		len,
	bool			*exists)
{
	union xfs_btree_irec	low;
	union xfs_btree_irec	high;

	memset(&low, 0, sizeof(low));
	low.a.ar_startblock = bno;
	memset(&high, 0xFF, sizeof(high));
	high.a.ar_startblock = bno + len - 1;

	return xfs_btree_has_record(cur, &low, &high, exists);
}