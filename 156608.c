xfs_alloc_ag_vextent(
	xfs_alloc_arg_t	*args)	/* argument structure for allocation */
{
	int		error=0;

	ASSERT(args->minlen > 0);
	ASSERT(args->maxlen > 0);
	ASSERT(args->minlen <= args->maxlen);
	ASSERT(args->mod < args->prod);
	ASSERT(args->alignment > 0);

	/*
	 * Branch to correct routine based on the type.
	 */
	args->wasfromfl = 0;
	switch (args->type) {
	case XFS_ALLOCTYPE_THIS_AG:
		error = xfs_alloc_ag_vextent_size(args);
		break;
	case XFS_ALLOCTYPE_NEAR_BNO:
		error = xfs_alloc_ag_vextent_near(args);
		break;
	case XFS_ALLOCTYPE_THIS_BNO:
		error = xfs_alloc_ag_vextent_exact(args);
		break;
	default:
		ASSERT(0);
		/* NOTREACHED */
	}

	if (error || args->agbno == NULLAGBLOCK)
		return error;

	ASSERT(args->len >= args->minlen);
	ASSERT(args->len <= args->maxlen);
	ASSERT(!args->wasfromfl || args->resv != XFS_AG_RESV_AGFL);
	ASSERT(args->agbno % args->alignment == 0);

	/* if not file data, insert new block into the reverse map btree */
	if (!xfs_rmap_should_skip_owner_update(&args->oinfo)) {
		error = xfs_rmap_alloc(args->tp, args->agbp, args->agno,
				       args->agbno, args->len, &args->oinfo);
		if (error)
			return error;
	}

	if (!args->wasfromfl) {
		error = xfs_alloc_update_counters(args->tp, args->pag,
						  args->agbp,
						  -((long)(args->len)));
		if (error)
			return error;

		ASSERT(!xfs_extent_busy_search(args->mp, args->agno,
					      args->agbno, args->len));
	}

	xfs_ag_resv_alloc_extent(args->pag, args->resv, args);

	XFS_STATS_INC(args->mp, xs_allocx);
	XFS_STATS_ADD(args->mp, xs_allocb, args->len);
	return error;
}