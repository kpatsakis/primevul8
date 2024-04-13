xfs_alloc_ag_vextent_locality(
	struct xfs_alloc_arg	*args,
	struct xfs_alloc_cur	*acur,
	int			*stat)
{
	struct xfs_btree_cur	*fbcur = NULL;
	int			error;
	int			i;
	bool			fbinc;

	ASSERT(acur->len == 0);
	ASSERT(args->type == XFS_ALLOCTYPE_NEAR_BNO);

	*stat = 0;

	error = xfs_alloc_lookup_ge(acur->cnt, args->agbno, acur->cur_len, &i);
	if (error)
		return error;
	error = xfs_alloc_lookup_le(acur->bnolt, args->agbno, 0, &i);
	if (error)
		return error;
	error = xfs_alloc_lookup_ge(acur->bnogt, args->agbno, 0, &i);
	if (error)
		return error;

	/*
	 * Search the bnobt and cntbt in parallel. Search the bnobt left and
	 * right and lookup the closest extent to the locality hint for each
	 * extent size key in the cntbt. The entire search terminates
	 * immediately on a bnobt hit because that means we've found best case
	 * locality. Otherwise the search continues until the cntbt cursor runs
	 * off the end of the tree. If no allocation candidate is found at this
	 * point, give up on locality, walk backwards from the end of the cntbt
	 * and take the first available extent.
	 *
	 * The parallel tree searches balance each other out to provide fairly
	 * consistent performance for various situations. The bnobt search can
	 * have pathological behavior in the worst case scenario of larger
	 * allocation requests and fragmented free space. On the other hand, the
	 * bnobt is able to satisfy most smaller allocation requests much more
	 * quickly than the cntbt. The cntbt search can sift through fragmented
	 * free space and sets of free extents for larger allocation requests
	 * more quickly than the bnobt. Since the locality hint is just a hint
	 * and we don't want to scan the entire bnobt for perfect locality, the
	 * cntbt search essentially bounds the bnobt search such that we can
	 * find good enough locality at reasonable performance in most cases.
	 */
	while (xfs_alloc_cur_active(acur->bnolt) ||
	       xfs_alloc_cur_active(acur->bnogt) ||
	       xfs_alloc_cur_active(acur->cnt)) {

		trace_xfs_alloc_cur_lookup(args);

		/*
		 * Search the bnobt left and right. In the case of a hit, finish
		 * the search in the opposite direction and we're done.
		 */
		error = xfs_alloc_walk_iter(args, acur, acur->bnolt, false,
					    true, 1, &i);
		if (error)
			return error;
		if (i == 1) {
			trace_xfs_alloc_cur_left(args);
			fbcur = acur->bnogt;
			fbinc = true;
			break;
		}
		error = xfs_alloc_walk_iter(args, acur, acur->bnogt, true, true,
					    1, &i);
		if (error)
			return error;
		if (i == 1) {
			trace_xfs_alloc_cur_right(args);
			fbcur = acur->bnolt;
			fbinc = false;
			break;
		}

		/*
		 * Check the extent with best locality based on the current
		 * extent size search key and keep track of the best candidate.
		 */
		error = xfs_alloc_cntbt_iter(args, acur);
		if (error)
			return error;
		if (!xfs_alloc_cur_active(acur->cnt)) {
			trace_xfs_alloc_cur_lookup_done(args);
			break;
		}
	}

	/*
	 * If we failed to find anything due to busy extents, return empty
	 * handed so the caller can flush and retry. If no busy extents were
	 * found, walk backwards from the end of the cntbt as a last resort.
	 */
	if (!xfs_alloc_cur_active(acur->cnt) && !acur->len && !acur->busy) {
		error = xfs_btree_decrement(acur->cnt, 0, &i);
		if (error)
			return error;
		if (i) {
			acur->cnt->bc_private.a.priv.abt.active = true;
			fbcur = acur->cnt;
			fbinc = false;
		}
	}

	/*
	 * Search in the opposite direction for a better entry in the case of
	 * a bnobt hit or walk backwards from the end of the cntbt.
	 */
	if (fbcur) {
		error = xfs_alloc_walk_iter(args, acur, fbcur, fbinc, true, -1,
					    &i);
		if (error)
			return error;
	}

	if (acur->len)
		*stat = 1;

	return 0;
}