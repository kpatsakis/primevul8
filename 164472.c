op_change(oparg_T *oap)
{
    colnr_T		l;
    int			retval;
    long		offset;
    linenr_T		linenr;
    long		ins_len;
    long		pre_textlen = 0;
    long		pre_indent = 0;
    char_u		*firstline;
    char_u		*ins_text, *newp, *oldp;
    struct block_def	bd;

    l = oap->start.col;
    if (oap->motion_type == MLINE)
    {
	l = 0;
	can_si = may_do_si();	// Like opening a new line, do smart indent
    }

    // First delete the text in the region.  In an empty buffer only need to
    // save for undo
    if (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	if (u_save_cursor() == FAIL)
	    return FALSE;
    }
    else if (op_delete(oap) == FAIL)
	return FALSE;

    if ((l > curwin->w_cursor.col) && !LINEEMPTY(curwin->w_cursor.lnum)
							 && !virtual_op)
	inc_cursor();

    // check for still on same line (<CR> in inserted text meaningless)
    // skip blank lines too
    if (oap->block_mode)
    {
	// Add spaces before getting the current line length.
	if (virtual_op && (curwin->w_cursor.coladd > 0
						    || gchar_cursor() == NUL))
	    coladvance_force(getviscol());
	firstline = ml_get(oap->start.lnum);
	pre_textlen = (long)STRLEN(firstline);
	pre_indent = (long)getwhitecols(firstline);
	bd.textcol = curwin->w_cursor.col;
    }

    if (oap->motion_type == MLINE)
	fix_indent();

    retval = edit(NUL, FALSE, (linenr_T)1);

    /*
     * In Visual block mode, handle copying the new text to all lines of the
     * block.
     * Don't repeat the insert when Insert mode ended with CTRL-C.
     */
    if (oap->block_mode && oap->start.lnum != oap->end.lnum && !got_int)
    {
	// Auto-indenting may have changed the indent.  If the cursor was past
	// the indent, exclude that indent change from the inserted text.
	firstline = ml_get(oap->start.lnum);
	if (bd.textcol > (colnr_T)pre_indent)
	{
	    long new_indent = (long)getwhitecols(firstline);

	    pre_textlen += new_indent - pre_indent;
	    bd.textcol += new_indent - pre_indent;
	}

	ins_len = (long)STRLEN(firstline) - pre_textlen;
	if (ins_len > 0)
	{
	    // Subsequent calls to ml_get() flush the firstline data - take a
	    // copy of the inserted text.
	    if ((ins_text = alloc(ins_len + 1)) != NULL)
	    {
		vim_strncpy(ins_text, firstline + bd.textcol, (size_t)ins_len);
		for (linenr = oap->start.lnum + 1; linenr <= oap->end.lnum;
								     linenr++)
		{
		    block_prep(oap, &bd, linenr, TRUE);
		    if (!bd.is_short || virtual_op)
		    {
			pos_T vpos;

			// If the block starts in virtual space, count the
			// initial coladd offset as part of "startspaces"
			if (bd.is_short)
			{
			    vpos.lnum = linenr;
			    (void)getvpos(&vpos, oap->start_vcol);
			}
			else
			    vpos.coladd = 0;
			oldp = ml_get(linenr);
			newp = alloc(STRLEN(oldp) + vpos.coladd + ins_len + 1);
			if (newp == NULL)
			    continue;
			// copy up to block start
			mch_memmove(newp, oldp, (size_t)bd.textcol);
			offset = bd.textcol;
			vim_memset(newp + offset, ' ', (size_t)vpos.coladd);
			offset += vpos.coladd;
			mch_memmove(newp + offset, ins_text, (size_t)ins_len);
			offset += ins_len;
			oldp += bd.textcol;
			STRMOVE(newp + offset, oldp);
			ml_replace(linenr, newp, FALSE);
#ifdef FEAT_PROP_POPUP
			// Shift the properties for linenr as edit() would do.
			if (curbuf->b_has_textprop)
			    adjust_prop_columns(linenr, bd.textcol,
						     vpos.coladd + ins_len, 0);
#endif
		    }
		}
		check_cursor();

		changed_lines(oap->start.lnum + 1, 0, oap->end.lnum + 1, 0L);
	    }
	    vim_free(ins_text);
	}
    }
    auto_format(FALSE, TRUE);

    return retval;
}