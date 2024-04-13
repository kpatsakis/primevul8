xfs_alloc_fixup_trees(
	xfs_btree_cur_t	*cnt_cur,	/* cursor for by-size btree */
	xfs_btree_cur_t	*bno_cur,	/* cursor for by-block btree */
	xfs_agblock_t	fbno,		/* starting block of free extent */
	xfs_extlen_t	flen,		/* length of free extent */
	xfs_agblock_t	rbno,		/* starting block of returned extent */
	xfs_extlen_t	rlen,		/* length of returned extent */
	int		flags)		/* flags, XFSA_FIXUP_... */
{
	int		error;		/* error code */
	int		i;		/* operation results */
	xfs_agblock_t	nfbno1;		/* first new free startblock */
	xfs_agblock_t	nfbno2;		/* second new free startblock */
	xfs_extlen_t	nflen1=0;	/* first new free length */
	xfs_extlen_t	nflen2=0;	/* second new free length */
	struct xfs_mount *mp;

	mp = cnt_cur->bc_mp;

	/*
	 * Look up the record in the by-size tree if necessary.
	 */
	if (flags & XFSA_FIXUP_CNT_OK) {
#ifdef DEBUG
		if ((error = xfs_alloc_get_rec(cnt_cur, &nfbno1, &nflen1, &i)))
			return error;
		if (XFS_IS_CORRUPT(mp,
				   i != 1 ||
				   nfbno1 != fbno ||
				   nflen1 != flen))
			return -EFSCORRUPTED;
#endif
	} else {
		if ((error = xfs_alloc_lookup_eq(cnt_cur, fbno, flen, &i)))
			return error;
		if (XFS_IS_CORRUPT(mp, i != 1))
			return -EFSCORRUPTED;
	}
	/*
	 * Look up the record in the by-block tree if necessary.
	 */
	if (flags & XFSA_FIXUP_BNO_OK) {
#ifdef DEBUG
		if ((error = xfs_alloc_get_rec(bno_cur, &nfbno1, &nflen1, &i)))
			return error;
		if (XFS_IS_CORRUPT(mp,
				   i != 1 ||
				   nfbno1 != fbno ||
				   nflen1 != flen))
			return -EFSCORRUPTED;
#endif
	} else {
		if ((error = xfs_alloc_lookup_eq(bno_cur, fbno, flen, &i)))
			return error;
		if (XFS_IS_CORRUPT(mp, i != 1))
			return -EFSCORRUPTED;
	}

#ifdef DEBUG
	if (bno_cur->bc_nlevels == 1 && cnt_cur->bc_nlevels == 1) {
		struct xfs_btree_block	*bnoblock;
		struct xfs_btree_block	*cntblock;

		bnoblock = XFS_BUF_TO_BLOCK(bno_cur->bc_bufs[0]);
		cntblock = XFS_BUF_TO_BLOCK(cnt_cur->bc_bufs[0]);

		if (XFS_IS_CORRUPT(mp,
				   bnoblock->bb_numrecs !=
				   cntblock->bb_numrecs))
			return -EFSCORRUPTED;
	}
#endif

	/*
	 * Deal with all four cases: the allocated record is contained
	 * within the freespace record, so we can have new freespace
	 * at either (or both) end, or no freespace remaining.
	 */
	if (rbno == fbno && rlen == flen)
		nfbno1 = nfbno2 = NULLAGBLOCK;
	else if (rbno == fbno) {
		nfbno1 = rbno + rlen;
		nflen1 = flen - rlen;
		nfbno2 = NULLAGBLOCK;
	} else if (rbno + rlen == fbno + flen) {
		nfbno1 = fbno;
		nflen1 = flen - rlen;
		nfbno2 = NULLAGBLOCK;
	} else {
		nfbno1 = fbno;
		nflen1 = rbno - fbno;
		nfbno2 = rbno + rlen;
		nflen2 = (fbno + flen) - nfbno2;
	}
	/*
	 * Delete the entry from the by-size btree.
	 */
	if ((error = xfs_btree_delete(cnt_cur, &i)))
		return error;
	if (XFS_IS_CORRUPT(mp, i != 1))
		return -EFSCORRUPTED;
	/*
	 * Add new by-size btree entry(s).
	 */
	if (nfbno1 != NULLAGBLOCK) {
		if ((error = xfs_alloc_lookup_eq(cnt_cur, nfbno1, nflen1, &i)))
			return error;
		if (XFS_IS_CORRUPT(mp, i != 0))
			return -EFSCORRUPTED;
		if ((error = xfs_btree_insert(cnt_cur, &i)))
			return error;
		if (XFS_IS_CORRUPT(mp, i != 1))
			return -EFSCORRUPTED;
	}
	if (nfbno2 != NULLAGBLOCK) {
		if ((error = xfs_alloc_lookup_eq(cnt_cur, nfbno2, nflen2, &i)))
			return error;
		if (XFS_IS_CORRUPT(mp, i != 0))
			return -EFSCORRUPTED;
		if ((error = xfs_btree_insert(cnt_cur, &i)))
			return error;
		if (XFS_IS_CORRUPT(mp, i != 1))
			return -EFSCORRUPTED;
	}
	/*
	 * Fix up the by-block btree entry(s).
	 */
	if (nfbno1 == NULLAGBLOCK) {
		/*
		 * No remaining freespace, just delete the by-block tree entry.
		 */
		if ((error = xfs_btree_delete(bno_cur, &i)))
			return error;
		if (XFS_IS_CORRUPT(mp, i != 1))
			return -EFSCORRUPTED;
	} else {
		/*
		 * Update the by-block entry to start later|be shorter.
		 */
		if ((error = xfs_alloc_update(bno_cur, nfbno1, nflen1)))
			return error;
	}
	if (nfbno2 != NULLAGBLOCK) {
		/*
		 * 2 resulting free entries, need to add one.
		 */
		if ((error = xfs_alloc_lookup_eq(bno_cur, nfbno2, nflen2, &i)))
			return error;
		if (XFS_IS_CORRUPT(mp, i != 0))
			return -EFSCORRUPTED;
		if ((error = xfs_btree_insert(bno_cur, &i)))
			return error;
		if (XFS_IS_CORRUPT(mp, i != 1))
			return -EFSCORRUPTED;
	}
	return 0;
}