xfs_alloc_longest_free_extent(
	struct xfs_perag	*pag,
	xfs_extlen_t		need,
	xfs_extlen_t		reserved)
{
	xfs_extlen_t		delta = 0;

	/*
	 * If the AGFL needs a recharge, we'll have to subtract that from the
	 * longest extent.
	 */
	if (need > pag->pagf_flcount)
		delta = need - pag->pagf_flcount;

	/*
	 * If we cannot maintain others' reservations with space from the
	 * not-longest freesp extents, we'll have to subtract /that/ from
	 * the longest extent too.
	 */
	if (pag->pagf_freeblks - pag->pagf_longest < reserved)
		delta += reserved - (pag->pagf_freeblks - pag->pagf_longest);

	/*
	 * If the longest extent is long enough to satisfy all the
	 * reservations and AGFL rules in place, we can return this extent.
	 */
	if (pag->pagf_longest > delta)
		return min_t(xfs_extlen_t, pag->pag_mount->m_ag_max_usable,
				pag->pagf_longest - delta);

	/* Otherwise, let the caller try for 1 block if there's space. */
	return pag->pagf_flcount > 0 || pag->pagf_longest > 0;
}