xfs_alloc_query_all(
	struct xfs_btree_cur			*cur,
	xfs_alloc_query_range_fn		fn,
	void					*priv)
{
	struct xfs_alloc_query_range_info	query;

	ASSERT(cur->bc_btnum == XFS_BTNUM_BNO);
	query.priv = priv;
	query.fn = fn;
	return xfs_btree_query_all(cur, xfs_alloc_query_range_helper, &query);
}