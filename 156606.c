xfs_free_agfl_block(
	struct xfs_trans	*tp,
	xfs_agnumber_t		agno,
	xfs_agblock_t		agbno,
	struct xfs_buf		*agbp,
	struct xfs_owner_info	*oinfo)
{
	int			error;
	struct xfs_buf		*bp;

	error = xfs_free_ag_extent(tp, agbp, agno, agbno, 1, oinfo,
				   XFS_AG_RESV_AGFL);
	if (error)
		return error;

	error = xfs_trans_get_buf(tp, tp->t_mountp->m_ddev_targp,
			XFS_AGB_TO_DADDR(tp->t_mountp, agno, agbno),
			tp->t_mountp->m_bsize, 0, &bp);
	if (error)
		return error;
	xfs_trans_binval(tp, bp);

	return 0;
}