xfs_alloc_query_range(
	struct xfs_btree_cur			*cur,
	struct xfs_alloc_rec_incore		*low_rec,
	struct xfs_alloc_rec_incore		*high_rec,
	xfs_alloc_query_range_fn		fn,
	void					*priv)
{
	union xfs_btree_irec			low_brec;
	union xfs_btree_irec			high_brec;
	struct xfs_alloc_query_range_info	query;

	ASSERT(cur->bc_btnum == XFS_BTNUM_BNO);
	low_brec.a = *low_rec;
	high_brec.a = *high_rec;
	query.priv = priv;
	query.fn = fn;
	return xfs_btree_query_range(cur, &low_brec, &high_brec,
			xfs_alloc_query_range_helper, &query);
}