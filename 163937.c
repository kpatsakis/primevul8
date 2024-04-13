xfs_attr_match(
	struct xfs_da_args	*args,
	uint8_t			namelen,
	unsigned char		*name,
	int			flags)
{
	if (args->namelen != namelen)
		return false;
	if (memcmp(args->name, name, namelen) != 0)
		return false;
	/*
	 * If we are looking for incomplete entries, show only those, else only
	 * show complete entries.
	 */
	if (args->attr_filter !=
	    (flags & (XFS_ATTR_NSP_ONDISK_MASK | XFS_ATTR_INCOMPLETE)))
		return false;
	return true;
}