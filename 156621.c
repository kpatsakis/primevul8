__xfs_free_extent(
	struct xfs_trans		*tp,
	xfs_fsblock_t			bno,
	xfs_extlen_t			len,
	const struct xfs_owner_info	*oinfo,
	enum xfs_ag_resv_type		type,
	bool				skip_discard)
{
	struct xfs_mount		*mp = tp->t_mountp;
	struct xfs_buf			*agbp;
	xfs_agnumber_t			agno = XFS_FSB_TO_AGNO(mp, bno);
	xfs_agblock_t			agbno = XFS_FSB_TO_AGBNO(mp, bno);
	int				error;
	unsigned int			busy_flags = 0;

	ASSERT(len != 0);
	ASSERT(type != XFS_AG_RESV_AGFL);

	if (XFS_TEST_ERROR(false, mp,
			XFS_ERRTAG_FREE_EXTENT))
		return -EIO;

	error = xfs_free_extent_fix_freelist(tp, agno, &agbp);
	if (error)
		return error;

	if (XFS_IS_CORRUPT(mp, agbno >= mp->m_sb.sb_agblocks)) {
		error = -EFSCORRUPTED;
		goto err;
	}

	/* validate the extent size is legal now we have the agf locked */
	if (XFS_IS_CORRUPT(mp,
			   agbno + len >
			   be32_to_cpu(XFS_BUF_TO_AGF(agbp)->agf_length))) {
		error = -EFSCORRUPTED;
		goto err;
	}

	error = xfs_free_ag_extent(tp, agbp, agno, agbno, len, oinfo, type);
	if (error)
		goto err;

	if (skip_discard)
		busy_flags |= XFS_EXTENT_BUSY_SKIP_DISCARD;
	xfs_extent_busy_insert(tp, agno, agbno, len, busy_flags);
	return 0;

err:
	xfs_trans_brelse(tp, agbp);
	return error;
}