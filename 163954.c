xfs_attr_copy_value(
	struct xfs_da_args	*args,
	unsigned char		*value,
	int			valuelen)
{
	/*
	 * No copy if all we have to do is get the length
	 */
	if (!args->valuelen) {
		args->valuelen = valuelen;
		return 0;
	}

	/*
	 * No copy if the length of the existing buffer is too small
	 */
	if (args->valuelen < valuelen) {
		args->valuelen = valuelen;
		return -ERANGE;
	}

	if (!args->value) {
		args->value = kmem_alloc_large(valuelen, KM_NOLOCKDEP);
		if (!args->value)
			return -ENOMEM;
	}
	args->valuelen = valuelen;

	/* remote block xattr requires IO for copy-in */
	if (args->rmtblkno)
		return xfs_attr_rmtval_get(args);

	/*
	 * This is to prevent a GCC warning because the remote xattr case
	 * doesn't have a value to pass in. In that case, we never reach here,
	 * but GCC can't work that out and so throws a "passing NULL to
	 * memcpy" warning.
	 */
	if (!value)
		return -EINVAL;
	memcpy(args->value, value, valuelen);
	return 0;
}