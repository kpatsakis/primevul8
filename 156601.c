xfs_alloc_fix_len(
	xfs_alloc_arg_t	*args)		/* allocation argument structure */
{
	xfs_extlen_t	k;
	xfs_extlen_t	rlen;

	ASSERT(args->mod < args->prod);
	rlen = args->len;
	ASSERT(rlen >= args->minlen);
	ASSERT(rlen <= args->maxlen);
	if (args->prod <= 1 || rlen < args->mod || rlen == args->maxlen ||
	    (args->mod == 0 && rlen < args->prod))
		return;
	k = rlen % args->prod;
	if (k == args->mod)
		return;
	if (k > args->mod)
		rlen = rlen - (k - args->mod);
	else
		rlen = rlen - args->prod + (args->mod - k);
	/* casts to (int) catch length underflows */
	if ((int)rlen < (int)args->minlen)
		return;
	ASSERT(rlen >= args->minlen && rlen <= args->maxlen);
	ASSERT(rlen % args->prod == args->mod);
	ASSERT(args->pag->pagf_freeblks + args->pag->pagf_flcount >=
		rlen + args->minleft);
	args->len = rlen;
}