xfs_alloc_cur_finish(
	struct xfs_alloc_arg	*args,
	struct xfs_alloc_cur	*acur)
{
	int			error;

	ASSERT(acur->cnt && acur->bnolt);
	ASSERT(acur->bno >= acur->rec_bno);
	ASSERT(acur->bno + acur->len <= acur->rec_bno + acur->rec_len);
	ASSERT(acur->rec_bno + acur->rec_len <=
	       be32_to_cpu(XFS_BUF_TO_AGF(args->agbp)->agf_length));

	error = xfs_alloc_fixup_trees(acur->cnt, acur->bnolt, acur->rec_bno,
				      acur->rec_len, acur->bno, acur->len, 0);
	if (error)
		return error;

	args->agbno = acur->bno;
	args->len = acur->len;
	args->wasfromfl = 0;

	trace_xfs_alloc_cur(args);
	return 0;
}