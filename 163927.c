xfs_attr_shortform_remove(
	struct xfs_da_args		*args)
{
	struct xfs_attr_shortform	*sf;
	struct xfs_attr_sf_entry	*sfe;
	int				size = 0, end, totsize;
	unsigned int			base;
	struct xfs_mount		*mp;
	struct xfs_inode		*dp;
	int				error;

	trace_xfs_attr_sf_remove(args);

	dp = args->dp;
	mp = dp->i_mount;
	sf = (xfs_attr_shortform_t *)dp->i_afp->if_u1.if_data;

	error = xfs_attr_sf_findname(args, &sfe, &base);
	if (error != -EEXIST)
		return error;
	size = XFS_ATTR_SF_ENTSIZE(sfe);

	/*
	 * Fix up the attribute fork data, covering the hole
	 */
	end = base + size;
	totsize = be16_to_cpu(sf->hdr.totsize);
	if (end != totsize)
		memmove(&((char *)sf)[base], &((char *)sf)[end], totsize - end);
	sf->hdr.count--;
	be16_add_cpu(&sf->hdr.totsize, -size);

	/*
	 * Fix up the start offset of the attribute fork
	 */
	totsize -= size;
	if (totsize == sizeof(xfs_attr_sf_hdr_t) &&
	    (mp->m_flags & XFS_MOUNT_ATTR2) &&
	    (dp->i_df.if_format != XFS_DINODE_FMT_BTREE) &&
	    !(args->op_flags & XFS_DA_OP_ADDNAME)) {
		xfs_attr_fork_remove(dp, args->trans);
	} else {
		xfs_idata_realloc(dp, -size, XFS_ATTR_FORK);
		dp->i_d.di_forkoff = xfs_attr_shortform_bytesfit(dp, totsize);
		ASSERT(dp->i_d.di_forkoff);
		ASSERT(totsize > sizeof(xfs_attr_sf_hdr_t) ||
				(args->op_flags & XFS_DA_OP_ADDNAME) ||
				!(mp->m_flags & XFS_MOUNT_ATTR2) ||
				dp->i_df.if_format == XFS_DINODE_FMT_BTREE);
		xfs_trans_log_inode(args->trans, dp,
					XFS_ILOG_CORE | XFS_ILOG_ADATA);
	}

	xfs_sbversion_add_attr2(mp, args->trans);

	return 0;
}