xfs_alloc_lookup_ge(
	struct xfs_btree_cur	*cur,	/* btree cursor */
	xfs_agblock_t		bno,	/* starting block of extent */
	xfs_extlen_t		len,	/* length of extent */
	int			*stat)	/* success/failure */
{
	int			error;

	cur->bc_rec.a.ar_startblock = bno;
	cur->bc_rec.a.ar_blockcount = len;
	error = xfs_btree_lookup(cur, XFS_LOOKUP_GE, stat);
	cur->bc_private.a.priv.abt.active = (*stat == 1);
	return error;
}