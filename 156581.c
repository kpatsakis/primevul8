xfs_alloc_get_rec(
	struct xfs_btree_cur	*cur,	/* btree cursor */
	xfs_agblock_t		*bno,	/* output: starting block of extent */
	xfs_extlen_t		*len,	/* output: length of extent */
	int			*stat)	/* output: success/failure */
{
	struct xfs_mount	*mp = cur->bc_mp;
	xfs_agnumber_t		agno = cur->bc_private.a.agno;
	union xfs_btree_rec	*rec;
	int			error;

	error = xfs_btree_get_rec(cur, &rec, stat);
	if (error || !(*stat))
		return error;

	*bno = be32_to_cpu(rec->alloc.ar_startblock);
	*len = be32_to_cpu(rec->alloc.ar_blockcount);

	if (*len == 0)
		goto out_bad_rec;

	/* check for valid extent range, including overflow */
	if (!xfs_verify_agbno(mp, agno, *bno))
		goto out_bad_rec;
	if (*bno > *bno + *len)
		goto out_bad_rec;
	if (!xfs_verify_agbno(mp, agno, *bno + *len - 1))
		goto out_bad_rec;

	return 0;

out_bad_rec:
	xfs_warn(mp,
		"%s Freespace BTree record corruption in AG %d detected!",
		cur->bc_btnum == XFS_BTNUM_BNO ? "Block" : "Size", agno);
	xfs_warn(mp,
		"start block 0x%x block count 0x%x", *bno, *len);
	return -EFSCORRUPTED;
}