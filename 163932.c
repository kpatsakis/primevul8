xfs_attr_shortform_getvalue(
	struct xfs_da_args	*args)
{
	struct xfs_attr_shortform *sf;
	struct xfs_attr_sf_entry *sfe;
	int			i;

	ASSERT(args->dp->i_afp->if_flags == XFS_IFINLINE);
	sf = (xfs_attr_shortform_t *)args->dp->i_afp->if_u1.if_data;
	sfe = &sf->list[0];
	for (i = 0; i < sf->hdr.count;
				sfe = XFS_ATTR_SF_NEXTENTRY(sfe), i++) {
		if (xfs_attr_match(args, sfe->namelen, sfe->nameval,
				sfe->flags))
			return xfs_attr_copy_value(args,
				&sfe->nameval[args->namelen], sfe->valuelen);
	}
	return -ENOATTR;
}