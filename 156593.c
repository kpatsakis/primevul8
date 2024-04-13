xfs_alloc_cur_setup(
	struct xfs_alloc_arg	*args,
	struct xfs_alloc_cur	*acur)
{
	int			error;
	int			i;

	ASSERT(args->alignment == 1 || args->type != XFS_ALLOCTYPE_THIS_BNO);

	acur->cur_len = args->maxlen;
	acur->rec_bno = 0;
	acur->rec_len = 0;
	acur->bno = 0;
	acur->len = 0;
	acur->diff = -1;
	acur->busy = false;
	acur->busy_gen = 0;

	/*
	 * Perform an initial cntbt lookup to check for availability of maxlen
	 * extents. If this fails, we'll return -ENOSPC to signal the caller to
	 * attempt a small allocation.
	 */
	if (!acur->cnt)
		acur->cnt = xfs_allocbt_init_cursor(args->mp, args->tp,
					args->agbp, args->agno, XFS_BTNUM_CNT);
	error = xfs_alloc_lookup_ge(acur->cnt, 0, args->maxlen, &i);
	if (error)
		return error;

	/*
	 * Allocate the bnobt left and right search cursors.
	 */
	if (!acur->bnolt)
		acur->bnolt = xfs_allocbt_init_cursor(args->mp, args->tp,
					args->agbp, args->agno, XFS_BTNUM_BNO);
	if (!acur->bnogt)
		acur->bnogt = xfs_allocbt_init_cursor(args->mp, args->tp,
					args->agbp, args->agno, XFS_BTNUM_BNO);
	return i == 1 ? 0 : -ENOSPC;
}