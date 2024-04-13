xfs_alloc_cntbt_iter(
	struct xfs_alloc_arg		*args,
	struct xfs_alloc_cur		*acur)
{
	struct xfs_btree_cur	*cur = acur->cnt;
	xfs_agblock_t		bno;
	xfs_extlen_t		len, cur_len;
	int			error;
	int			i;

	if (!xfs_alloc_cur_active(cur))
		return 0;

	/* locality optimized lookup */
	cur_len = acur->cur_len;
	error = xfs_alloc_lookup_ge(cur, args->agbno, cur_len, &i);
	if (error)
		return error;
	if (i == 0)
		return 0;
	error = xfs_alloc_get_rec(cur, &bno, &len, &i);
	if (error)
		return error;

	/* check the current record and update search length from it */
	error = xfs_alloc_cur_check(args, acur, cur, &i);
	if (error)
		return error;
	ASSERT(len >= acur->cur_len);
	acur->cur_len = len;

	/*
	 * We looked up the first record >= [agbno, len] above. The agbno is a
	 * secondary key and so the current record may lie just before or after
	 * agbno. If it is past agbno, check the previous record too so long as
	 * the length matches as it may be closer. Don't check a smaller record
	 * because that could deactivate our cursor.
	 */
	if (bno > args->agbno) {
		error = xfs_btree_decrement(cur, 0, &i);
		if (!error && i) {
			error = xfs_alloc_get_rec(cur, &bno, &len, &i);
			if (!error && i && len == acur->cur_len)
				error = xfs_alloc_cur_check(args, acur, cur,
							    &i);
		}
		if (error)
			return error;
	}

	/*
	 * Increment the search key until we find at least one allocation
	 * candidate or if the extent we found was larger. Otherwise, double the
	 * search key to optimize the search. Efficiency is more important here
	 * than absolute best locality.
	 */
	cur_len <<= 1;
	if (!acur->len || acur->cur_len >= cur_len)
		acur->cur_len++;
	else
		acur->cur_len = cur_len;

	return error;
}