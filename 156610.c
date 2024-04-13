xfs_alloc_pagf_init(
	xfs_mount_t		*mp,	/* file system mount structure */
	xfs_trans_t		*tp,	/* transaction pointer */
	xfs_agnumber_t		agno,	/* allocation group number */
	int			flags)	/* XFS_ALLOC_FLAGS_... */
{
	xfs_buf_t		*bp;
	int			error;

	error = xfs_alloc_read_agf(mp, tp, agno, flags, &bp);
	if (!error)
		xfs_trans_brelse(tp, bp);
	return error;
}