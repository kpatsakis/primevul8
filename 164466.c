op_tilde(oparg_T *oap)
{
    pos_T		pos;
    struct block_def	bd;
    int			did_change = FALSE;

    if (u_save((linenr_T)(oap->start.lnum - 1),
				       (linenr_T)(oap->end.lnum + 1)) == FAIL)
	return;

    pos = oap->start;
    if (oap->block_mode)		    // Visual block mode
    {
	for (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    int one_change;

	    block_prep(oap, &bd, pos.lnum, FALSE);
	    pos.col = bd.textcol;
	    one_change = swapchars(oap->op_type, &pos, bd.textlen);
	    did_change |= one_change;

#ifdef FEAT_NETBEANS_INTG
	    if (netbeans_active() && one_change)
	    {
		char_u *ptr;

		netbeans_removed(curbuf, pos.lnum, bd.textcol,
							    (long)bd.textlen);
		// get the line now, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, bd.textcol,
						&ptr[bd.textcol], bd.textlen);
	    }
#endif
	}
	if (did_change)
	    changed_lines(oap->start.lnum, 0, oap->end.lnum + 1, 0L);
    }
    else				    // not block mode
    {
	if (oap->motion_type == MLINE)
	{
	    oap->start.col = 0;
	    pos.col = 0;
	    oap->end.col = (colnr_T)STRLEN(ml_get(oap->end.lnum));
	    if (oap->end.col)
		--oap->end.col;
	}
	else if (!oap->inclusive)
	    dec(&(oap->end));

	if (pos.lnum == oap->end.lnum)
	    did_change = swapchars(oap->op_type, &pos,
						  oap->end.col - pos.col + 1);
	else
	    for (;;)
	    {
		did_change |= swapchars(oap->op_type, &pos,
				pos.lnum == oap->end.lnum ? oap->end.col + 1:
					   (int)STRLEN(ml_get_pos(&pos)));
		if (LTOREQ_POS(oap->end, pos) || inc(&pos) == -1)
		    break;
	    }
	if (did_change)
	{
	    changed_lines(oap->start.lnum, oap->start.col, oap->end.lnum + 1,
									  0L);
#ifdef FEAT_NETBEANS_INTG
	    if (netbeans_active())
	    {
		char_u *ptr;
		int count;

		pos = oap->start;
		while (pos.lnum < oap->end.lnum)
		{
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    count = (int)STRLEN(ptr) - pos.col;
		    netbeans_removed(curbuf, pos.lnum, pos.col, (long)count);
		    // get the line again, it may have been flushed
		    ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		    netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], count);
		    pos.col = 0;
		    pos.lnum++;
		}
		count = oap->end.col - pos.col + 1;
		netbeans_removed(curbuf, pos.lnum, pos.col, (long)count);
		// get the line again, it may have been flushed
		ptr = ml_get_buf(curbuf, pos.lnum, FALSE);
		netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], count);
	    }
#endif
	}
    }

    if (!did_change && oap->is_VIsual)
	// No change: need to remove the Visual selection
	redraw_curbuf_later(UPD_INVERTED);

    if ((cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// Set '[ and '] marks.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
    }

    if (oap->line_count > p_report)
	smsg(NGETTEXT("%ld line changed", "%ld lines changed",
					    oap->line_count), oap->line_count);
}