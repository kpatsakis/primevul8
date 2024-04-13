xfs_alloc_vextent(
	struct xfs_alloc_arg	*args)	/* allocation argument structure */
{
	xfs_agblock_t		agsize;	/* allocation group size */
	int			error;
	int			flags;	/* XFS_ALLOC_FLAG_... locking flags */
	struct xfs_mount	*mp;	/* mount structure pointer */
	xfs_agnumber_t		sagno;	/* starting allocation group number */
	xfs_alloctype_t		type;	/* input allocation type */
	int			bump_rotor = 0;
	xfs_agnumber_t		rotorstep = xfs_rotorstep; /* inode32 agf stepper */

	mp = args->mp;
	type = args->otype = args->type;
	args->agbno = NULLAGBLOCK;
	/*
	 * Just fix this up, for the case where the last a.g. is shorter
	 * (or there's only one a.g.) and the caller couldn't easily figure
	 * that out (xfs_bmap_alloc).
	 */
	agsize = mp->m_sb.sb_agblocks;
	if (args->maxlen > agsize)
		args->maxlen = agsize;
	if (args->alignment == 0)
		args->alignment = 1;
	ASSERT(XFS_FSB_TO_AGNO(mp, args->fsbno) < mp->m_sb.sb_agcount);
	ASSERT(XFS_FSB_TO_AGBNO(mp, args->fsbno) < agsize);
	ASSERT(args->minlen <= args->maxlen);
	ASSERT(args->minlen <= agsize);
	ASSERT(args->mod < args->prod);
	if (XFS_FSB_TO_AGNO(mp, args->fsbno) >= mp->m_sb.sb_agcount ||
	    XFS_FSB_TO_AGBNO(mp, args->fsbno) >= agsize ||
	    args->minlen > args->maxlen || args->minlen > agsize ||
	    args->mod >= args->prod) {
		args->fsbno = NULLFSBLOCK;
		trace_xfs_alloc_vextent_badargs(args);
		return 0;
	}

	switch (type) {
	case XFS_ALLOCTYPE_THIS_AG:
	case XFS_ALLOCTYPE_NEAR_BNO:
	case XFS_ALLOCTYPE_THIS_BNO:
		/*
		 * These three force us into a single a.g.
		 */
		args->agno = XFS_FSB_TO_AGNO(mp, args->fsbno);
		args->pag = xfs_perag_get(mp, args->agno);
		error = xfs_alloc_fix_freelist(args, 0);
		if (error) {
			trace_xfs_alloc_vextent_nofix(args);
			goto error0;
		}
		if (!args->agbp) {
			trace_xfs_alloc_vextent_noagbp(args);
			break;
		}
		args->agbno = XFS_FSB_TO_AGBNO(mp, args->fsbno);
		if ((error = xfs_alloc_ag_vextent(args)))
			goto error0;
		break;
	case XFS_ALLOCTYPE_START_BNO:
		/*
		 * Try near allocation first, then anywhere-in-ag after
		 * the first a.g. fails.
		 */
		if ((args->datatype & XFS_ALLOC_INITIAL_USER_DATA) &&
		    (mp->m_flags & XFS_MOUNT_32BITINODES)) {
			args->fsbno = XFS_AGB_TO_FSB(mp,
					((mp->m_agfrotor / rotorstep) %
					mp->m_sb.sb_agcount), 0);
			bump_rotor = 1;
		}
		args->agbno = XFS_FSB_TO_AGBNO(mp, args->fsbno);
		args->type = XFS_ALLOCTYPE_NEAR_BNO;
		/* FALLTHROUGH */
	case XFS_ALLOCTYPE_FIRST_AG:
		/*
		 * Rotate through the allocation groups looking for a winner.
		 */
		if (type == XFS_ALLOCTYPE_FIRST_AG) {
			/*
			 * Start with allocation group given by bno.
			 */
			args->agno = XFS_FSB_TO_AGNO(mp, args->fsbno);
			args->type = XFS_ALLOCTYPE_THIS_AG;
			sagno = 0;
			flags = 0;
		} else {
			/*
			 * Start with the given allocation group.
			 */
			args->agno = sagno = XFS_FSB_TO_AGNO(mp, args->fsbno);
			flags = XFS_ALLOC_FLAG_TRYLOCK;
		}
		/*
		 * Loop over allocation groups twice; first time with
		 * trylock set, second time without.
		 */
		for (;;) {
			args->pag = xfs_perag_get(mp, args->agno);
			error = xfs_alloc_fix_freelist(args, flags);
			if (error) {
				trace_xfs_alloc_vextent_nofix(args);
				goto error0;
			}
			/*
			 * If we get a buffer back then the allocation will fly.
			 */
			if (args->agbp) {
				if ((error = xfs_alloc_ag_vextent(args)))
					goto error0;
				break;
			}

			trace_xfs_alloc_vextent_loopfailed(args);

			/*
			 * Didn't work, figure out the next iteration.
			 */
			if (args->agno == sagno &&
			    type == XFS_ALLOCTYPE_START_BNO)
				args->type = XFS_ALLOCTYPE_THIS_AG;
			/*
			* For the first allocation, we can try any AG to get
			* space.  However, if we already have allocated a
			* block, we don't want to try AGs whose number is below
			* sagno. Otherwise, we may end up with out-of-order
			* locking of AGF, which might cause deadlock.
			*/
			if (++(args->agno) == mp->m_sb.sb_agcount) {
				if (args->tp->t_firstblock != NULLFSBLOCK)
					args->agno = sagno;
				else
					args->agno = 0;
			}
			/*
			 * Reached the starting a.g., must either be done
			 * or switch to non-trylock mode.
			 */
			if (args->agno == sagno) {
				if (flags == 0) {
					args->agbno = NULLAGBLOCK;
					trace_xfs_alloc_vextent_allfailed(args);
					break;
				}

				flags = 0;
				if (type == XFS_ALLOCTYPE_START_BNO) {
					args->agbno = XFS_FSB_TO_AGBNO(mp,
						args->fsbno);
					args->type = XFS_ALLOCTYPE_NEAR_BNO;
				}
			}
			xfs_perag_put(args->pag);
		}
		if (bump_rotor) {
			if (args->agno == sagno)
				mp->m_agfrotor = (mp->m_agfrotor + 1) %
					(mp->m_sb.sb_agcount * rotorstep);
			else
				mp->m_agfrotor = (args->agno * rotorstep + 1) %
					(mp->m_sb.sb_agcount * rotorstep);
		}
		break;
	default:
		ASSERT(0);
		/* NOTREACHED */
	}
	if (args->agbno == NULLAGBLOCK)
		args->fsbno = NULLFSBLOCK;
	else {
		args->fsbno = XFS_AGB_TO_FSB(mp, args->agno, args->agbno);
#ifdef DEBUG
		ASSERT(args->len >= args->minlen);
		ASSERT(args->len <= args->maxlen);
		ASSERT(args->agbno % args->alignment == 0);
		XFS_AG_CHECK_DADDR(mp, XFS_FSB_TO_DADDR(mp, args->fsbno),
			args->len);
#endif

	}
	xfs_perag_put(args->pag);
	return 0;
error0:
	xfs_perag_put(args->pag);
	return error;
}