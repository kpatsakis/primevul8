xfs_attr_shortform_create(
	struct xfs_da_args	*args)
{
	struct xfs_inode	*dp = args->dp;
	struct xfs_ifork	*ifp = dp->i_afp;
	struct xfs_attr_sf_hdr	*hdr;

	trace_xfs_attr_sf_create(args);

	ASSERT(ifp->if_bytes == 0);
	if (ifp->if_format == XFS_DINODE_FMT_EXTENTS) {
		ifp->if_flags &= ~XFS_IFEXTENTS;	/* just in case */
		ifp->if_format = XFS_DINODE_FMT_LOCAL;
		ifp->if_flags |= XFS_IFINLINE;
	} else {
		ASSERT(ifp->if_flags & XFS_IFINLINE);
	}
	xfs_idata_realloc(dp, sizeof(*hdr), XFS_ATTR_FORK);
	hdr = (xfs_attr_sf_hdr_t *)ifp->if_u1.if_data;
	hdr->count = 0;
	hdr->totsize = cpu_to_be16(sizeof(*hdr));
	xfs_trans_log_inode(args->trans, dp, XFS_ILOG_CORE | XFS_ILOG_ADATA);
}