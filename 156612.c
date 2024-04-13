xfs_alloc_cur_close(
	struct xfs_alloc_cur	*acur,
	bool			error)
{
	int			cur_error = XFS_BTREE_NOERROR;

	if (error)
		cur_error = XFS_BTREE_ERROR;

	if (acur->cnt)
		xfs_btree_del_cursor(acur->cnt, cur_error);
	if (acur->bnolt)
		xfs_btree_del_cursor(acur->bnolt, cur_error);
	if (acur->bnogt)
		xfs_btree_del_cursor(acur->bnogt, cur_error);
	acur->cnt = acur->bnolt = acur->bnogt = NULL;
}