xfs_alloc_log_agf(
	xfs_trans_t	*tp,	/* transaction pointer */
	xfs_buf_t	*bp,	/* buffer for a.g. freelist header */
	int		fields)	/* mask of fields to be logged (XFS_AGF_...) */
{
	int	first;		/* first byte offset */
	int	last;		/* last byte offset */
	static const short	offsets[] = {
		offsetof(xfs_agf_t, agf_magicnum),
		offsetof(xfs_agf_t, agf_versionnum),
		offsetof(xfs_agf_t, agf_seqno),
		offsetof(xfs_agf_t, agf_length),
		offsetof(xfs_agf_t, agf_roots[0]),
		offsetof(xfs_agf_t, agf_levels[0]),
		offsetof(xfs_agf_t, agf_flfirst),
		offsetof(xfs_agf_t, agf_fllast),
		offsetof(xfs_agf_t, agf_flcount),
		offsetof(xfs_agf_t, agf_freeblks),
		offsetof(xfs_agf_t, agf_longest),
		offsetof(xfs_agf_t, agf_btreeblks),
		offsetof(xfs_agf_t, agf_uuid),
		offsetof(xfs_agf_t, agf_rmap_blocks),
		offsetof(xfs_agf_t, agf_refcount_blocks),
		offsetof(xfs_agf_t, agf_refcount_root),
		offsetof(xfs_agf_t, agf_refcount_level),
		/* needed so that we don't log the whole rest of the structure: */
		offsetof(xfs_agf_t, agf_spare64),
		sizeof(xfs_agf_t)
	};

	trace_xfs_agf(tp->t_mountp, XFS_BUF_TO_AGF(bp), fields, _RET_IP_);

	xfs_trans_buf_set_type(tp, bp, XFS_BLFT_AGF_BUF);

	xfs_btree_offsets(fields, offsets, XFS_AGF_NUM_BITS, &first, &last);
	xfs_trans_log_buf(tp, bp, (uint)first, (uint)last);
}