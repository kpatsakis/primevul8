op_colon(oparg_T *oap)
{
    stuffcharReadbuff(':');
    if (oap->is_VIsual)
	stuffReadbuff((char_u *)"'<,'>");
    else
    {
	// Make the range look nice, so it can be repeated.
	if (oap->start.lnum == curwin->w_cursor.lnum)
	    stuffcharReadbuff('.');
	else
	    stuffnumReadbuff((long)oap->start.lnum);
	if (oap->end.lnum != oap->start.lnum)
	{
	    stuffcharReadbuff(',');
	    if (oap->end.lnum == curwin->w_cursor.lnum)
		stuffcharReadbuff('.');
	    else if (oap->end.lnum == curbuf->b_ml.ml_line_count)
		stuffcharReadbuff('$');
	    else if (oap->start.lnum == curwin->w_cursor.lnum)
	    {
		stuffReadbuff((char_u *)".+");
		stuffnumReadbuff((long)oap->line_count - 1);
	    }
	    else
		stuffnumReadbuff((long)oap->end.lnum);
	}
    }
    if (oap->op_type != OP_COLON)
	stuffReadbuff((char_u *)"!");
    if (oap->op_type == OP_INDENT)
    {
	if (*get_equalprg() == NUL)
	    stuffReadbuff((char_u *)"indent");
	else
	    stuffReadbuff(get_equalprg());
	stuffReadbuff((char_u *)"\n");
    }
    else if (oap->op_type == OP_FORMAT)
    {
	if (*curbuf->b_p_fp != NUL)
	    stuffReadbuff(curbuf->b_p_fp);
	else if (*p_fp != NUL)
	    stuffReadbuff(p_fp);
	else
	    stuffReadbuff((char_u *)"fmt");
	stuffReadbuff((char_u *)"\n']");
    }

    // do_cmdline() does the rest
}