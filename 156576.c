xfs_alloc_ag_vextent_small(
	struct xfs_alloc_arg	*args,	/* allocation argument structure */
	struct xfs_btree_cur	*ccur,	/* optional by-size cursor */
	xfs_agblock_t		*fbnop,	/* result block number */
	xfs_extlen_t		*flenp,	/* result length */
	int			*stat)	/* status: 0-freelist, 1-normal/none */
{
	int			error = 0;
	xfs_agblock_t		fbno = NULLAGBLOCK;
	xfs_extlen_t		flen = 0;
	int			i = 0;

	/*
	 * If a cntbt cursor is provided, try to allocate the largest record in
	 * the tree. Try the AGFL if the cntbt is empty, otherwise fail the
	 * allocation. Make sure to respect minleft even when pulling from the
	 * freelist.
	 */
	if (ccur)
		error = xfs_btree_decrement(ccur, 0, &i);
	if (error)
		goto error;
	if (i) {
		error = xfs_alloc_get_rec(ccur, &fbno, &flen, &i);
		if (error)
			goto error;
		if (XFS_IS_CORRUPT(args->mp, i != 1)) {
			error = -EFSCORRUPTED;
			goto error;
		}
		goto out;
	}

	if (args->minlen != 1 || args->alignment != 1 ||
	    args->resv == XFS_AG_RESV_AGFL ||
	    (be32_to_cpu(XFS_BUF_TO_AGF(args->agbp)->agf_flcount) <=
	     args->minleft))
		goto out;

	error = xfs_alloc_get_freelist(args->tp, args->agbp, &fbno, 0);
	if (error)
		goto error;
	if (fbno == NULLAGBLOCK)
		goto out;

	xfs_extent_busy_reuse(args->mp, args->agno, fbno, 1,
			      (args->datatype & XFS_ALLOC_NOBUSY));

	if (args->datatype & XFS_ALLOC_USERDATA) {
		struct xfs_buf	*bp;

		error = xfs_trans_get_buf(args->tp, args->mp->m_ddev_targp,
				XFS_AGB_TO_DADDR(args->mp, args->agno, fbno),
				args->mp->m_bsize, 0, &bp);
		if (error)
			goto error;
		xfs_trans_binval(args->tp, bp);
	}
	*fbnop = args->agbno = fbno;
	*flenp = args->len = 1;
	if (XFS_IS_CORRUPT(args->mp,
			   fbno >= be32_to_cpu(
				   XFS_BUF_TO_AGF(args->agbp)->agf_length))) {
		error = -EFSCORRUPTED;
		goto error;
	}
	args->wasfromfl = 1;
	trace_xfs_alloc_small_freelist(args);

	/*
	 * If we're feeding an AGFL block to something that doesn't live in the
	 * free space, we need to clear out the OWN_AG rmap.
	 */
	error = xfs_rmap_free(args->tp, args->agbp, args->agno, fbno, 1,
			      &XFS_RMAP_OINFO_AG);
	if (error)
		goto error;

	*stat = 0;
	return 0;

out:
	/*
	 * Can't do the allocation, give up.
	 */
	if (flen < args->minlen) {
		args->agbno = NULLAGBLOCK;
		trace_xfs_alloc_small_notenough(args);
		flen = 0;
	}
	*fbnop = fbno;
	*flenp = flen;
	*stat = 1;
	trace_xfs_alloc_small_done(args);
	return 0;

error:
	trace_xfs_alloc_small_error(args);
	return error;
}