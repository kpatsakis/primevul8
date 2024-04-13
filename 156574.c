xfs_alloc_query_range_helper(
	struct xfs_btree_cur		*cur,
	union xfs_btree_rec		*rec,
	void				*priv)
{
	struct xfs_alloc_query_range_info	*query = priv;
	struct xfs_alloc_rec_incore		irec;

	irec.ar_startblock = be32_to_cpu(rec->alloc.ar_startblock);
	irec.ar_blockcount = be32_to_cpu(rec->alloc.ar_blockcount);
	return query->fn(cur, &irec, query->priv);
}