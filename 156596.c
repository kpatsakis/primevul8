xfs_alloc_ag_vextent_lastblock(
	struct xfs_alloc_arg	*args,
	struct xfs_alloc_cur	*acur,
	xfs_agblock_t		*bno,
	xfs_extlen_t		*len,
	bool			*allocated)
{
	int			error;
	int			i;

#ifdef DEBUG
	/* Randomly don't execute the first algorithm. */
	if (prandom_u32() & 1)
		return 0;
#endif

	/*
	 * Start from the entry that lookup found, sequence through all larger
	 * free blocks.  If we're actually pointing at a record smaller than
	 * maxlen, go to the start of this block, and skip all those smaller
	 * than minlen.
	 */
	if (len || args->alignment > 1) {
		acur->cnt->bc_ptrs[0] = 1;
		do {
			error = xfs_alloc_get_rec(acur->cnt, bno, len, &i);
			if (error)
				return error;
			if (XFS_IS_CORRUPT(args->mp, i != 1))
				return -EFSCORRUPTED;
			if (*len >= args->minlen)
				break;
			error = xfs_btree_increment(acur->cnt, 0, &i);
			if (error)
				return error;
		} while (i);
		ASSERT(*len >= args->minlen);
		if (!i)
			return 0;
	}

	error = xfs_alloc_walk_iter(args, acur, acur->cnt, true, false, -1, &i);
	if (error)
		return error;

	/*
	 * It didn't work.  We COULD be in a case where there's a good record
	 * somewhere, so try again.
	 */
	if (acur->len == 0)
		return 0;

	trace_xfs_alloc_near_first(args);
	*allocated = true;
	return 0;
}