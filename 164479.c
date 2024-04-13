do_addsub(
    int		op_type,
    pos_T	*pos,
    int		length,
    linenr_T	Prenum1)
{
    int		col;
    char_u	*buf1;
    char_u	buf2[NUMBUFLEN];
    int		pre;		// 'X'/'x': hex; '0': octal; 'B'/'b': bin
    static int	hexupper = FALSE;	// 0xABC
    uvarnumber_T	n;
    uvarnumber_T	oldn;
    char_u	*ptr;
    int		c;
    int		todel;
    int		do_hex;
    int		do_oct;
    int		do_bin;
    int		do_alpha;
    int		do_unsigned;
    int		firstdigit;
    int		subtract;
    int		negative = FALSE;
    int		was_positive = TRUE;
    int		visual = VIsual_active;
    int		did_change = FALSE;
    pos_T	save_cursor = curwin->w_cursor;
    int		maxlen = 0;
    pos_T	startpos;
    pos_T	endpos;
    colnr_T	save_coladd = 0;

    do_hex = (vim_strchr(curbuf->b_p_nf, 'x') != NULL);	// "heX"
    do_oct = (vim_strchr(curbuf->b_p_nf, 'o') != NULL);	// "Octal"
    do_bin = (vim_strchr(curbuf->b_p_nf, 'b') != NULL);	// "Bin"
    do_alpha = (vim_strchr(curbuf->b_p_nf, 'p') != NULL);	// "alPha"
    do_unsigned = (vim_strchr(curbuf->b_p_nf, 'u') != NULL);	// "Unsigned"

    if (virtual_active())
    {
	save_coladd = pos->coladd;
	pos->coladd = 0;
    }

    curwin->w_cursor = *pos;
    ptr = ml_get(pos->lnum);
    col = pos->col;

    if (*ptr == NUL || col + !!save_coladd >= (int)STRLEN(ptr))
	goto theend;

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    if (!VIsual_active)
    {
	if (do_bin)
	    while (col > 0 && vim_isbdigit(ptr[col]))
	    {
		--col;
		if (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	if (do_hex)
	    while (col > 0 && vim_isxdigit(ptr[col]))
	    {
		--col;
		if (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }

	if (       do_bin
		&& do_hex
		&& ! ((col > 0
		    && (ptr[col] == 'X'
			|| ptr[col] == 'x')
		    && ptr[col - 1] == '0'
		    && (!has_mbyte ||
			!(*mb_head_off)(ptr, ptr + col - 1))
		    && vim_isxdigit(ptr[col + 1]))))
	{

	    // In case of binary/hexadecimal pattern overlap match, rescan

	    col = pos->col;

	    while (col > 0 && vim_isdigit(ptr[col]))
	    {
		col--;
		if (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}

	if ((       do_hex
		&& col > 0
		&& (ptr[col] == 'X'
		    || ptr[col] == 'x')
		&& ptr[col - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_head_off)(ptr, ptr + col - 1))
		&& vim_isxdigit(ptr[col + 1])) ||
	    (       do_bin
		&& col > 0
		&& (ptr[col] == 'B'
		    || ptr[col] == 'b')
		&& ptr[col - 1] == '0'
		&& (!has_mbyte ||
		    !(*mb_head_off)(ptr, ptr + col - 1))
		&& vim_isbdigit(ptr[col + 1])))
	{
	    // Found hexadecimal or binary number, move to its start.
	    --col;
	    if (has_mbyte)
		col -= (*mb_head_off)(ptr, ptr + col);
	}
	else
	{
	    /*
	     * Search forward and then backward to find the start of number.
	     */
	    col = pos->col;

	    while (ptr[col] != NUL
		    && !vim_isdigit(ptr[col])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
		col += mb_ptr2len(ptr + col);

	    while (col > 0
		    && vim_isdigit(ptr[col - 1])
		    && !(do_alpha && ASCII_ISALPHA(ptr[col])))
	    {
		--col;
		if (has_mbyte)
		    col -= (*mb_head_off)(ptr, ptr + col);
	    }
	}
    }

    if (visual)
    {
	while (ptr[col] != NUL && length > 0
		&& !vim_isdigit(ptr[col])
		&& !(do_alpha && ASCII_ISALPHA(ptr[col])))
	{
	    int mb_len = mb_ptr2len(ptr + col);

	    col += mb_len;
	    length -= mb_len;
	}

	if (length == 0)
	    goto theend;

	if (col > pos->col && ptr[col - 1] == '-'
		&& (!has_mbyte || !(*mb_head_off)(ptr, ptr + col - 1))
		&& !do_unsigned)
	{
	    negative = TRUE;
	    was_positive = FALSE;
	}
    }

    /*
     * If a number was found, and saving for undo works, replace the number.
     */
    firstdigit = ptr[col];
    if (!VIM_ISDIGIT(firstdigit) && !(do_alpha && ASCII_ISALPHA(firstdigit)))
    {
	beep_flush();
	goto theend;
    }

    if (do_alpha && ASCII_ISALPHA(firstdigit))
    {
	// decrement or increment alphabetic character
	if (op_type == OP_NR_SUB)
	{
	    if (CharOrd(firstdigit) < Prenum1)
	    {
		if (isupper(firstdigit))
		    firstdigit = 'A';
		else
		    firstdigit = 'a';
	    }
	    else
		firstdigit -= Prenum1;
	}
	else
	{
	    if (26 - CharOrd(firstdigit) - 1 < Prenum1)
	    {
		if (isupper(firstdigit))
		    firstdigit = 'Z';
		else
		    firstdigit = 'z';
	    }
	    else
		firstdigit += Prenum1;
	}
	curwin->w_cursor.col = col;
	if (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	(void)del_char(FALSE);
	ins_char(firstdigit);
	endpos = curwin->w_cursor;
	curwin->w_cursor.col = col;
    }
    else
    {
	pos_T	save_pos;
	int	i;

	if (col > 0 && ptr[col - 1] == '-'
		&& (!has_mbyte ||
		    !(*mb_head_off)(ptr, ptr + col - 1))
		&& !visual
		&& !do_unsigned)
	{
	    // negative number
	    --col;
	    negative = TRUE;
	}
	// get the number value (unsigned)
	if (visual && VIsual_mode != 'V')
	    maxlen = (curbuf->b_visual.vi_curswant == MAXCOL
		    ? (int)STRLEN(ptr) - col
		    : length);

	vim_str2nr(ptr + col, &pre, &length,
		0 + (do_bin ? STR2NR_BIN : 0)
		    + (do_oct ? STR2NR_OCT : 0)
		    + (do_hex ? STR2NR_HEX : 0),
		NULL, &n, maxlen, FALSE);

	// ignore leading '-' for hex and octal and bin numbers
	if (pre && negative)
	{
	    ++col;
	    --length;
	    negative = FALSE;
	}
	// add or subtract
	subtract = FALSE;
	if (op_type == OP_NR_SUB)
	    subtract ^= TRUE;
	if (negative)
	    subtract ^= TRUE;

	oldn = n;
	if (subtract)
	    n -= (uvarnumber_T)Prenum1;
	else
	    n += (uvarnumber_T)Prenum1;
	// handle wraparound for decimal numbers
	if (!pre)
	{
	    if (subtract)
	    {
		if (n > oldn)
		{
		    n = 1 + (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    else
	    {
		// add
		if (n < oldn)
		{
		    n = (n ^ (uvarnumber_T)-1);
		    negative ^= TRUE;
		}
	    }
	    if (n == 0)
		negative = FALSE;
	}

	if (do_unsigned && negative)
	{
	    if (subtract)
		// sticking at zero.
		n = (uvarnumber_T)0;
	    else
		// sticking at 2^64 - 1.
		n = (uvarnumber_T)(-1);
	    negative = FALSE;
	}

	if (visual && !was_positive && !negative && col > 0)
	{
	    // need to remove the '-'
	    col--;
	    length++;
	}

	/*
	 * Delete the old number.
	 */
	curwin->w_cursor.col = col;
	if (!did_change)
	    startpos = curwin->w_cursor;
	did_change = TRUE;
	todel = length;
	c = gchar_cursor();
	/*
	 * Don't include the '-' in the length, only the length of the
	 * part after it is kept the same.
	 */
	if (c == '-')
	    --length;

	save_pos = curwin->w_cursor;
	for (i = 0; i < todel; ++i)
	{
	    if (c < 0x100 && isalpha(c))
	    {
		if (isupper(c))
		    hexupper = TRUE;
		else
		    hexupper = FALSE;
	    }
	    inc_cursor();
	    c = gchar_cursor();
	}
	curwin->w_cursor = save_pos;

	/*
	 * Prepare the leading characters in buf1[].
	 * When there are many leading zeros it could be very long.
	 * Allocate a bit too much.
	 */
	buf1 = alloc(length + NUMBUFLEN);
	if (buf1 == NULL)
	    goto theend;
	ptr = buf1;
	if (negative && (!visual || was_positive))
	    *ptr++ = '-';
	if (pre)
	{
	    *ptr++ = '0';
	    --length;
	}
	if (pre == 'b' || pre == 'B' ||
	    pre == 'x' || pre == 'X')
	{
	    *ptr++ = pre;
	    --length;
	}

	/*
	 * Put the number characters in buf2[].
	 */
	if (pre == 'b' || pre == 'B')
	{
	    int bit = 0;
	    int bits = sizeof(uvarnumber_T) * 8;

	    // leading zeros
	    for (bit = bits; bit > 0; bit--)
		if ((n >> (bit - 1)) & 0x1) break;

	    for (i = 0; bit > 0; bit--)
		buf2[i++] = ((n >> (bit - 1)) & 0x1) ? '1' : '0';

	    buf2[i] = '\0';
	}
	else if (pre == 0)
	    vim_snprintf((char *)buf2, NUMBUFLEN, "%llu", n);
	else if (pre == '0')
	    vim_snprintf((char *)buf2, NUMBUFLEN, "%llo", n);
	else if (pre && hexupper)
	    vim_snprintf((char *)buf2, NUMBUFLEN, "%llX", n);
	else
	    vim_snprintf((char *)buf2, NUMBUFLEN, "%llx", n);
	length -= (int)STRLEN(buf2);

	/*
	 * Adjust number of zeros to the new number of digits, so the
	 * total length of the number remains the same.
	 * Don't do this when
	 * the result may look like an octal number.
	 */
	if (firstdigit == '0' && !(do_oct && pre == 0))
	    while (length-- > 0)
		*ptr++ = '0';
	*ptr = NUL;

	STRCAT(buf1, buf2);

	// Insert just after the first character to be removed, so that any
	// text properties will be adjusted.  Then delete the old number
	// afterwards.
	save_pos = curwin->w_cursor;
	if (todel > 0)
	    inc_cursor();
	ins_str(buf1);		// insert the new number
	vim_free(buf1);

	// del_char() will also mark line needing displaying
	if (todel > 0)
	{
	    int bytes_after = (int)STRLEN(ml_get_curline())
							- curwin->w_cursor.col;

	    // Delete the one character before the insert.
	    curwin->w_cursor = save_pos;
	    (void)del_char(FALSE);
	    curwin->w_cursor.col = (colnr_T)(STRLEN(ml_get_curline())
								- bytes_after);
	    --todel;
	}
	while (todel-- > 0)
	    (void)del_char(FALSE);

	endpos = curwin->w_cursor;
	if (did_change && curwin->w_cursor.col)
	    --curwin->w_cursor.col;
    }

    if (did_change && (cmdmod.cmod_flags & CMOD_LOCKMARKS) == 0)
    {
	// set the '[ and '] marks
	curbuf->b_op_start = startpos;
	curbuf->b_op_end = endpos;
	if (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }

theend:
    if (visual)
	curwin->w_cursor = save_cursor;
    else if (did_change)
	curwin->w_set_curswant = TRUE;
    else if (virtual_active())
	curwin->w_cursor.coladd = save_coladd;

    return did_change;
}