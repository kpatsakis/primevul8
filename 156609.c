xfs_alloc_cur_active(
	struct xfs_btree_cur	*cur)
{
	return cur && cur->bc_private.a.priv.abt.active;
}