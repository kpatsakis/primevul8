xfs_agfl_reset(
	struct xfs_trans	*tp,
	struct xfs_buf		*agbp,
	struct xfs_perag	*pag)
{
	struct xfs_mount	*mp = tp->t_mountp;
	struct xfs_agf		*agf = XFS_BUF_TO_AGF(agbp);

	ASSERT(pag->pagf_agflreset);
	trace_xfs_agfl_reset(mp, agf, 0, _RET_IP_);

	xfs_warn(mp,
	       "WARNING: Reset corrupted AGFL on AG %u. %d blocks leaked. "
	       "Please unmount and run xfs_repair.",
	         pag->pag_agno, pag->pagf_flcount);

	agf->agf_flfirst = 0;
	agf->agf_fllast = cpu_to_be32(xfs_agfl_size(mp) - 1);
	agf->agf_flcount = 0;
	xfs_alloc_log_agf(tp, agbp, XFS_AGF_FLFIRST | XFS_AGF_FLLAST |
				    XFS_AGF_FLCOUNT);

	pag->pagf_flcount = 0;
	pag->pagf_agflreset = false;
}