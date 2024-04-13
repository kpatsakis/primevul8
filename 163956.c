xfs_attr_sf_findname(
	struct xfs_da_args	 *args,
	struct xfs_attr_sf_entry **sfep,
	unsigned int		 *basep)
{
	struct xfs_attr_shortform *sf;
	struct xfs_attr_sf_entry *sfe;
	unsigned int		base = sizeof(struct xfs_attr_sf_hdr);
	int			size = 0;
	int			end;
	int			i;

	sf = (struct xfs_attr_shortform *)args->dp->i_afp->if_u1.if_data;
	sfe = &sf->list[0];
	end = sf->hdr.count;
	for (i = 0; i < end; sfe = XFS_ATTR_SF_NEXTENTRY(sfe),
			     base += size, i++) {
		size = XFS_ATTR_SF_ENTSIZE(sfe);
		if (!xfs_attr_match(args, sfe->namelen, sfe->nameval,
				    sfe->flags))
			continue;
		break;
	}

	if (sfep != NULL)
		*sfep = sfe;

	if (basep != NULL)
		*basep = base;

	if (i == end)
		return -ENOATTR;
	return -EEXIST;
}