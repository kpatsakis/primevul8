xfs_defer_agfl_block(
	struct xfs_trans		*tp,
	xfs_agnumber_t			agno,
	xfs_fsblock_t			agbno,
	struct xfs_owner_info		*oinfo)
{
	struct xfs_mount		*mp = tp->t_mountp;
	struct xfs_extent_free_item	*new;		/* new element */

	ASSERT(xfs_bmap_free_item_zone != NULL);
	ASSERT(oinfo != NULL);

	new = kmem_zone_alloc(xfs_bmap_free_item_zone, 0);
	new->xefi_startblock = XFS_AGB_TO_FSB(mp, agno, agbno);
	new->xefi_blockcount = 1;
	new->xefi_oinfo = *oinfo;

	trace_xfs_agfl_free_defer(mp, agno, 0, agbno, 1);

	xfs_defer_add(tp, XFS_DEFER_OPS_TYPE_AGFL_FREE, &new->xefi_list);
}