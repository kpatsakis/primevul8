xfs_alloc_get_freelist(
	xfs_trans_t	*tp,	/* transaction pointer */
	xfs_buf_t	*agbp,	/* buffer containing the agf structure */
	xfs_agblock_t	*bnop,	/* block address retrieved from freelist */
	int		btreeblk) /* destination is a AGF btree */
{
	xfs_agf_t	*agf;	/* a.g. freespace structure */
	xfs_buf_t	*agflbp;/* buffer for a.g. freelist structure */
	xfs_agblock_t	bno;	/* block number returned */
	__be32		*agfl_bno;
	int		error;
	int		logflags;
	xfs_mount_t	*mp = tp->t_mountp;
	xfs_perag_t	*pag;	/* per allocation group data */

	/*
	 * Freelist is empty, give up.
	 */
	agf = XFS_BUF_TO_AGF(agbp);
	if (!agf->agf_flcount) {
		*bnop = NULLAGBLOCK;
		return 0;
	}
	/*
	 * Read the array of free blocks.
	 */
	error = xfs_alloc_read_agfl(mp, tp, be32_to_cpu(agf->agf_seqno),
				    &agflbp);
	if (error)
		return error;


	/*
	 * Get the block number and update the data structures.
	 */
	agfl_bno = XFS_BUF_TO_AGFL_BNO(mp, agflbp);
	bno = be32_to_cpu(agfl_bno[be32_to_cpu(agf->agf_flfirst)]);
	be32_add_cpu(&agf->agf_flfirst, 1);
	xfs_trans_brelse(tp, agflbp);
	if (be32_to_cpu(agf->agf_flfirst) == xfs_agfl_size(mp))
		agf->agf_flfirst = 0;

	pag = xfs_perag_get(mp, be32_to_cpu(agf->agf_seqno));
	ASSERT(!pag->pagf_agflreset);
	be32_add_cpu(&agf->agf_flcount, -1);
	xfs_trans_agflist_delta(tp, -1);
	pag->pagf_flcount--;

	logflags = XFS_AGF_FLFIRST | XFS_AGF_FLCOUNT;
	if (btreeblk) {
		be32_add_cpu(&agf->agf_btreeblks, 1);
		pag->pagf_btreeblks++;
		logflags |= XFS_AGF_BTREEBLKS;
	}
	xfs_perag_put(pag);

	xfs_alloc_log_agf(tp, agbp, logflags);
	*bnop = bno;

	return 0;
}