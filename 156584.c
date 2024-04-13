xfs_alloc_cur_check(
	struct xfs_alloc_arg	*args,
	struct xfs_alloc_cur	*acur,
	struct xfs_btree_cur	*cur,
	int			*new)
{
	int			error, i;
	xfs_agblock_t		bno, bnoa, bnew;
	xfs_extlen_t		len, lena, diff = -1;
	bool			busy;
	unsigned		busy_gen = 0;
	bool			deactivate = false;
	bool			isbnobt = cur->bc_btnum == XFS_BTNUM_BNO;

	*new = 0;

	error = xfs_alloc_get_rec(cur, &bno, &len, &i);
	if (error)
		return error;
	if (XFS_IS_CORRUPT(args->mp, i != 1))
		return -EFSCORRUPTED;

	/*
	 * Check minlen and deactivate a cntbt cursor if out of acceptable size
	 * range (i.e., walking backwards looking for a minlen extent).
	 */
	if (len < args->minlen) {
		deactivate = !isbnobt;
		goto out;
	}

	busy = xfs_alloc_compute_aligned(args, bno, len, &bnoa, &lena,
					 &busy_gen);
	acur->busy |= busy;
	if (busy)
		acur->busy_gen = busy_gen;
	/* deactivate a bnobt cursor outside of locality range */
	if (bnoa < args->min_agbno || bnoa > args->max_agbno) {
		deactivate = isbnobt;
		goto out;
	}
	if (lena < args->minlen)
		goto out;

	args->len = XFS_EXTLEN_MIN(lena, args->maxlen);
	xfs_alloc_fix_len(args);
	ASSERT(args->len >= args->minlen);
	if (args->len < acur->len)
		goto out;

	/*
	 * We have an aligned record that satisfies minlen and beats or matches
	 * the candidate extent size. Compare locality for near allocation mode.
	 */
	ASSERT(args->type == XFS_ALLOCTYPE_NEAR_BNO);
	diff = xfs_alloc_compute_diff(args->agbno, args->len,
				      args->alignment, args->datatype,
				      bnoa, lena, &bnew);
	if (bnew == NULLAGBLOCK)
		goto out;

	/*
	 * Deactivate a bnobt cursor with worse locality than the current best.
	 */
	if (diff > acur->diff) {
		deactivate = isbnobt;
		goto out;
	}

	ASSERT(args->len > acur->len ||
	       (args->len == acur->len && diff <= acur->diff));
	acur->rec_bno = bno;
	acur->rec_len = len;
	acur->bno = bnew;
	acur->len = args->len;
	acur->diff = diff;
	*new = 1;

	/*
	 * We're done if we found a perfect allocation. This only deactivates
	 * the current cursor, but this is just an optimization to terminate a
	 * cntbt search that otherwise runs to the edge of the tree.
	 */
	if (acur->diff == 0 && acur->len == args->maxlen)
		deactivate = true;
out:
	if (deactivate)
		cur->bc_private.a.priv.abt.active = false;
	trace_xfs_alloc_cur_check(args->mp, cur->bc_btnum, bno, len, diff,
				  *new);
	return 0;
}