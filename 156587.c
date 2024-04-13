xfs_alloc_walk_iter(
	struct xfs_alloc_arg	*args,
	struct xfs_alloc_cur	*acur,
	struct xfs_btree_cur	*cur,
	bool			increment,
	bool			find_one, /* quit on first candidate */
	int			count,    /* rec count (-1 for infinite) */
	int			*stat)
{
	int			error;
	int			i;

	*stat = 0;

	/*
	 * Search so long as the cursor is active or we find a better extent.
	 * The cursor is deactivated if it extends beyond the range of the
	 * current allocation candidate.
	 */
	while (xfs_alloc_cur_active(cur) && count) {
		error = xfs_alloc_cur_check(args, acur, cur, &i);
		if (error)
			return error;
		if (i == 1) {
			*stat = 1;
			if (find_one)
				break;
		}
		if (!xfs_alloc_cur_active(cur))
			break;

		if (increment)
			error = xfs_btree_increment(cur, 0, &i);
		else
			error = xfs_btree_decrement(cur, 0, &i);
		if (error)
			return error;
		if (i == 0)
			cur->bc_private.a.priv.abt.active = false;

		if (count > 0)
			count--;
	}

	return 0;
}