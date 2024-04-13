xfs_attr_leaf_newentsize(
	struct xfs_da_args	*args,
	int			*local)
{
	int			size;

	size = xfs_attr_leaf_entsize_local(args->namelen, args->valuelen);
	if (size < xfs_attr_leaf_entsize_local_max(args->geo->blksize)) {
		if (local)
			*local = 1;
		return size;
	}
	if (local)
		*local = 0;
	return xfs_attr_leaf_entsize_remote(args->namelen);
}