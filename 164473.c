cursor_pos_info(dict_T *dict)
{
    char_u	*p;
    char_u	buf1[50];
    char_u	buf2[40];
    linenr_T	lnum;
    varnumber_T	byte_count = 0;
    varnumber_T	bom_count  = 0;
    varnumber_T	byte_count_cursor = 0;
    varnumber_T	char_count = 0;
    varnumber_T	char_count_cursor = 0;
    varnumber_T	word_count = 0;
    varnumber_T	word_count_cursor = 0;
    int		eol_size;
    varnumber_T	last_check = 100000L;
    long	line_count_selected = 0;
    pos_T	min_pos, max_pos;
    oparg_T	oparg;
    struct block_def	bd;

    /*
     * Compute the length of the file in characters.
     */
    if (curbuf->b_ml.ml_flags & ML_EMPTY)
    {
	if (dict == NULL)
	{
	    msg(_(no_lines_msg));
	    return;
	}
    }
    else
    {
	if (get_fileformat(curbuf) == EOL_DOS)
	    eol_size = 2;
	else
	    eol_size = 1;

	if (VIsual_active)
	{
	    if (LT_POS(VIsual, curwin->w_cursor))
	    {
		min_pos = VIsual;
		max_pos = curwin->w_cursor;
	    }
	    else
	    {
		min_pos = curwin->w_cursor;
		max_pos = VIsual;
	    }
	    if (*p_sel == 'e' && max_pos.col > 0)
		--max_pos.col;

	    if (VIsual_mode == Ctrl_V)
	    {
#ifdef FEAT_LINEBREAK
		char_u * saved_sbr = p_sbr;
		char_u * saved_w_sbr = curwin->w_p_sbr;

		// Make 'sbr' empty for a moment to get the correct size.
		p_sbr = empty_option;
		curwin->w_p_sbr = empty_option;
#endif
		oparg.is_VIsual = 1;
		oparg.block_mode = TRUE;
		oparg.op_type = OP_NOP;
		getvcols(curwin, &min_pos, &max_pos,
					  &oparg.start_vcol, &oparg.end_vcol);
#ifdef FEAT_LINEBREAK
		p_sbr = saved_sbr;
		curwin->w_p_sbr = saved_w_sbr;
#endif
		if (curwin->w_curswant == MAXCOL)
		    oparg.end_vcol = MAXCOL;
		// Swap the start, end vcol if needed
		if (oparg.end_vcol < oparg.start_vcol)
		{
		    oparg.end_vcol += oparg.start_vcol;
		    oparg.start_vcol = oparg.end_vcol - oparg.start_vcol;
		    oparg.end_vcol -= oparg.start_vcol;
		}
	    }
	    line_count_selected = max_pos.lnum - min_pos.lnum + 1;
	}

	for (lnum = 1; lnum <= curbuf->b_ml.ml_line_count; ++lnum)
	{
	    // Check for a CTRL-C every 100000 characters.
	    if (byte_count > last_check)
	    {
		ui_breakcheck();
		if (got_int)
		    return;
		last_check = byte_count + 100000L;
	    }

	    // Do extra processing for VIsual mode.
	    if (VIsual_active
		    && lnum >= min_pos.lnum && lnum <= max_pos.lnum)
	    {
		char_u	    *s = NULL;
		long	    len = 0L;

		switch (VIsual_mode)
		{
		    case Ctrl_V:
			virtual_op = virtual_active();
			block_prep(&oparg, &bd, lnum, 0);
			virtual_op = MAYBE;
			s = bd.textstart;
			len = (long)bd.textlen;
			break;
		    case 'V':
			s = ml_get(lnum);
			len = MAXCOL;
			break;
		    case 'v':
			{
			    colnr_T start_col = (lnum == min_pos.lnum)
							   ? min_pos.col : 0;
			    colnr_T end_col = (lnum == max_pos.lnum)
				      ? max_pos.col - start_col + 1 : MAXCOL;

			    s = ml_get(lnum) + start_col;
			    len = end_col;
			}
			break;
		}
		if (s != NULL)
		{
		    byte_count_cursor += line_count_info(s, &word_count_cursor,
					   &char_count_cursor, len, eol_size);
		    if (lnum == curbuf->b_ml.ml_line_count
			    && !curbuf->b_p_eol
			    && (curbuf->b_p_bin || !curbuf->b_p_fixeol)
			    && (long)STRLEN(s) < len)
			byte_count_cursor -= eol_size;
		}
	    }
	    else
	    {
		// In non-visual mode, check for the line the cursor is on
		if (lnum == curwin->w_cursor.lnum)
		{
		    word_count_cursor += word_count;
		    char_count_cursor += char_count;
		    byte_count_cursor = byte_count +
			line_count_info(ml_get(lnum),
				&word_count_cursor, &char_count_cursor,
				(varnumber_T)(curwin->w_cursor.col + 1),
				eol_size);
		}
	    }
	    // Add to the running totals
	    byte_count += line_count_info(ml_get(lnum), &word_count,
					 &char_count, (varnumber_T)MAXCOL,
					 eol_size);
	}

	// Correction for when last line doesn't have an EOL.
	if (!curbuf->b_p_eol && (curbuf->b_p_bin || !curbuf->b_p_fixeol))
	    byte_count -= eol_size;

	if (dict == NULL)
	{
	    if (VIsual_active)
	    {
		if (VIsual_mode == Ctrl_V && curwin->w_curswant < MAXCOL)
		{
		    getvcols(curwin, &min_pos, &max_pos, &min_pos.col,
								    &max_pos.col);
		    vim_snprintf((char *)buf1, sizeof(buf1), _("%ld Cols; "),
			    (long)(oparg.end_vcol - oparg.start_vcol + 1));
		}
		else
		    buf1[0] = NUL;

		if (char_count_cursor == byte_count_cursor
						    && char_count == byte_count)
		    vim_snprintf((char *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Bytes"),
			    buf1, line_count_selected,
			    (long)curbuf->b_ml.ml_line_count,
			    word_count_cursor,
			    word_count,
			    byte_count_cursor,
			    byte_count);
		else
		    vim_snprintf((char *)IObuff, IOSIZE,
			    _("Selected %s%ld of %ld Lines; %lld of %lld Words; %lld of %lld Chars; %lld of %lld Bytes"),
			    buf1, line_count_selected,
			    (long)curbuf->b_ml.ml_line_count,
			    word_count_cursor,
			    word_count,
			    char_count_cursor,
			    char_count,
			    byte_count_cursor,
			    byte_count);
	    }
	    else
	    {
		p = ml_get_curline();
		validate_virtcol();
		col_print(buf1, sizeof(buf1), (int)curwin->w_cursor.col + 1,
			(int)curwin->w_virtcol + 1);
		col_print(buf2, sizeof(buf2), (int)STRLEN(p),
				    linetabsize(p));

		if (char_count_cursor == byte_count_cursor
			&& char_count == byte_count)
		    vim_snprintf((char *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Byte %lld of %lld"),
			(char *)buf1, (char *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_line_count,
			word_count_cursor, word_count,
			byte_count_cursor, byte_count);
		else
		    vim_snprintf((char *)IObuff, IOSIZE,
			_("Col %s of %s; Line %ld of %ld; Word %lld of %lld; Char %lld of %lld; Byte %lld of %lld"),
			(char *)buf1, (char *)buf2,
			(long)curwin->w_cursor.lnum,
			(long)curbuf->b_ml.ml_line_count,
			word_count_cursor, word_count,
			char_count_cursor, char_count,
			byte_count_cursor, byte_count);
	    }
	}

	bom_count = bomb_size();
	if (dict == NULL && bom_count > 0)
	{
	    size_t len = STRLEN(IObuff);

	    vim_snprintf((char *)IObuff + len, IOSIZE - len,
				 _("(+%lld for BOM)"), bom_count);
	}
	if (dict == NULL)
	{
	    // Don't shorten this message, the user asked for it.
	    p = p_shm;
	    p_shm = (char_u *)"";
	    msg((char *)IObuff);
	    p_shm = p;
	}
    }
#if defined(FEAT_EVAL)
    if (dict != NULL)
    {
	dict_add_number(dict, "words", word_count);
	dict_add_number(dict, "chars", char_count);
	dict_add_number(dict, "bytes", byte_count + bom_count);
	dict_add_number(dict, VIsual_active ? "visual_bytes" : "cursor_bytes",
		byte_count_cursor);
	dict_add_number(dict, VIsual_active ? "visual_chars" : "cursor_chars",
		char_count_cursor);
	dict_add_number(dict, VIsual_active ? "visual_words" : "cursor_words",
		word_count_cursor);
    }
#endif
}