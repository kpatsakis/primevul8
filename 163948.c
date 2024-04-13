xfs_attr_shortform_to_leaf(
	struct xfs_da_args		*args,
	struct xfs_buf			**leaf_bp)
{
	struct xfs_inode		*dp;
	struct xfs_attr_shortform	*sf;
	struct xfs_attr_sf_entry	*sfe;
	struct xfs_da_args		nargs;
	char				*tmpbuffer;
	int				error, i, size;
	xfs_dablk_t			blkno;
	struct xfs_buf			*bp;
	struct xfs_ifork		*ifp;

	trace_xfs_attr_sf_to_leaf(args);

	dp = args->dp;
	ifp = dp->i_afp;
	sf = (xfs_attr_shortform_t *)ifp->if_u1.if_data;
	size = be16_to_cpu(sf->hdr.totsize);
	tmpbuffer = kmem_alloc(size, 0);
	ASSERT(tmpbuffer != NULL);
	memcpy(tmpbuffer, ifp->if_u1.if_data, size);
	sf = (xfs_attr_shortform_t *)tmpbuffer;

	xfs_idata_realloc(dp, -size, XFS_ATTR_FORK);
	xfs_bmap_local_to_extents_empty(args->trans, dp, XFS_ATTR_FORK);

	bp = NULL;
	error = xfs_da_grow_inode(args, &blkno);
	if (error)
		goto out;

	ASSERT(blkno == 0);
	error = xfs_attr3_leaf_create(args, blkno, &bp);
	if (error)
		goto out;

	memset((char *)&nargs, 0, sizeof(nargs));
	nargs.dp = dp;
	nargs.geo = args->geo;
	nargs.total = args->total;
	nargs.whichfork = XFS_ATTR_FORK;
	nargs.trans = args->trans;
	nargs.op_flags = XFS_DA_OP_OKNOENT;

	sfe = &sf->list[0];
	for (i = 0; i < sf->hdr.count; i++) {
		nargs.name = sfe->nameval;
		nargs.namelen = sfe->namelen;
		nargs.value = &sfe->nameval[nargs.namelen];
		nargs.valuelen = sfe->valuelen;
		nargs.hashval = xfs_da_hashname(sfe->nameval,
						sfe->namelen);
		nargs.attr_filter = sfe->flags & XFS_ATTR_NSP_ONDISK_MASK;
		error = xfs_attr3_leaf_lookup_int(bp, &nargs); /* set a->index */
		ASSERT(error == -ENOATTR);
		error = xfs_attr3_leaf_add(bp, &nargs);
		ASSERT(error != -ENOSPC);
		if (error)
			goto out;
		sfe = XFS_ATTR_SF_NEXTENTRY(sfe);
	}
	error = 0;
	*leaf_bp = bp;
out:
	kmem_free(tmpbuffer);
	return error;
}