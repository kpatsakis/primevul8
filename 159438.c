nv_ident(cmdarg_T *cap)
{
    char_u	*ptr = NULL;
    char_u	*buf;
    unsigned	buflen;
    char_u	*newbuf;
    char_u	*p;
    char_u	*kp;		// value of 'keywordprg'
    int		kp_help;	// 'keywordprg' is ":he"
    int		kp_ex;		// 'keywordprg' starts with ":"
    int		n = 0;		// init for GCC
    int		cmdchar;
    int		g_cmd;		// "g" command
    int		tag_cmd = FALSE;
    char_u	*aux_ptr;
    int		isman;
    int		isman_s;

    if (cap->cmdchar == 'g')	// "g*", "g#", "g]" and "gCTRL-]"
    {
	cmdchar = cap->nchar;
	g_cmd = TRUE;
    }
    else
    {
	cmdchar = cap->cmdchar;
	g_cmd = FALSE;
    }

    if (cmdchar == POUND)	// the pound sign, '#' for English keyboards
	cmdchar = '#';

    /*
     * The "]", "CTRL-]" and "K" commands accept an argument in Visual mode.
     */
    if (cmdchar == ']' || cmdchar == Ctrl_RSB || cmdchar == 'K')
    {
	if (VIsual_active && get_visual_text(cap, &ptr, &n) == FAIL)
	    return;
	if (checkclearopq(cap->oap))
	    return;
    }

    if (ptr == NULL && (n = find_ident_under_cursor(&ptr,
		    (cmdchar == '*' || cmdchar == '#')
				 ? FIND_IDENT|FIND_STRING : FIND_IDENT)) == 0)
    {
	clearop(cap->oap);
	return;
    }

    // Allocate buffer to put the command in.  Inserting backslashes can
    // double the length of the word.  p_kp / curbuf->b_p_kp could be added
    // and some numbers.
    kp = (*curbuf->b_p_kp == NUL ? p_kp : curbuf->b_p_kp);
    kp_help = (*kp == NUL || STRCMP(kp, ":he") == 0
						 || STRCMP(kp, ":help") == 0);
    if (kp_help && *skipwhite(ptr) == NUL)
    {
	emsg(_(e_noident));	 // found white space only
	return;
    }
    kp_ex = (*kp == ':');
    buflen = (unsigned)(n * 2 + 30 + STRLEN(kp));
    buf = alloc(buflen);
    if (buf == NULL)
	return;
    buf[0] = NUL;

    switch (cmdchar)
    {
	case '*':
	case '#':
	    /*
	     * Put cursor at start of word, makes search skip the word
	     * under the cursor.
	     * Call setpcmark() first, so "*``" puts the cursor back where
	     * it was.
	     */
	    setpcmark();
	    curwin->w_cursor.col = (colnr_T) (ptr - ml_get_curline());

	    if (!g_cmd && vim_iswordp(ptr))
		STRCPY(buf, "\\<");
	    no_smartcase = TRUE;	// don't use 'smartcase' now
	    break;

	case 'K':
	    if (kp_help)
		STRCPY(buf, "he! ");
	    else if (kp_ex)
	    {
		if (cap->count0 != 0)
		    vim_snprintf((char *)buf, buflen, "%s %ld",
							     kp, cap->count0);
		else
		    STRCPY(buf, kp);
		STRCAT(buf, " ");
	    }
	    else
	    {
		// An external command will probably use an argument starting
		// with "-" as an option.  To avoid trouble we skip the "-".
		while (*ptr == '-' && n > 0)
		{
		    ++ptr;
		    --n;
		}
		if (n == 0)
		{
		    emsg(_(e_noident));	 // found dashes only
		    vim_free(buf);
		    return;
		}

		// When a count is given, turn it into a range.  Is this
		// really what we want?
		isman = (STRCMP(kp, "man") == 0);
		isman_s = (STRCMP(kp, "man -s") == 0);
		if (cap->count0 != 0 && !(isman || isman_s))
		    sprintf((char *)buf, ".,.+%ld", cap->count0 - 1);

		STRCAT(buf, "! ");
		if (cap->count0 == 0 && isman_s)
		    STRCAT(buf, "man");
		else
		    STRCAT(buf, kp);
		STRCAT(buf, " ");
		if (cap->count0 != 0 && (isman || isman_s))
		{
		    sprintf((char *)buf + STRLEN(buf), "%ld", cap->count0);
		    STRCAT(buf, " ");
		}
	    }
	    break;

	case ']':
	    tag_cmd = TRUE;
#ifdef FEAT_CSCOPE
	    if (p_cst)
		STRCPY(buf, "cstag ");
	    else
#endif
		STRCPY(buf, "ts ");
	    break;

	default:
	    tag_cmd = TRUE;
	    if (curbuf->b_help)
		STRCPY(buf, "he! ");
	    else
	    {
		if (g_cmd)
		    STRCPY(buf, "tj ");
		else if (cap->count0 == 0)
		    STRCPY(buf, "ta ");
		else
		    sprintf((char *)buf, ":%ldta ", cap->count0);
	    }
    }

    /*
     * Now grab the chars in the identifier
     */
    if (cmdchar == 'K' && !kp_help)
    {
	ptr = vim_strnsave(ptr, n);
	if (kp_ex)
	    // Escape the argument properly for an Ex command
	    p = vim_strsave_fnameescape(ptr, FALSE);
	else
	    // Escape the argument properly for a shell command
	    p = vim_strsave_shellescape(ptr, TRUE, TRUE);
	vim_free(ptr);
	if (p == NULL)
	{
	    vim_free(buf);
	    return;
	}
	newbuf = vim_realloc(buf, STRLEN(buf) + STRLEN(p) + 1);
	if (newbuf == NULL)
	{
	    vim_free(buf);
	    vim_free(p);
	    return;
	}
	buf = newbuf;
	STRCAT(buf, p);
	vim_free(p);
    }
    else
    {
	if (cmdchar == '*')
	    aux_ptr = (char_u *)(magic_isset() ? "/.*~[^$\\" : "/^$\\");
	else if (cmdchar == '#')
	    aux_ptr = (char_u *)(magic_isset() ? "/?.*~[^$\\" : "/?^$\\");
	else if (tag_cmd)
	{
	    if (curbuf->b_help)
		// ":help" handles unescaped argument
		aux_ptr = (char_u *)"";
	    else
		aux_ptr = (char_u *)"\\|\"\n[";
	}
	else
	    aux_ptr = (char_u *)"\\|\"\n*?[";

	p = buf + STRLEN(buf);
	while (n-- > 0)
	{
	    // put a backslash before \ and some others
	    if (vim_strchr(aux_ptr, *ptr) != NULL)
		*p++ = '\\';
	    // When current byte is a part of multibyte character, copy all
	    // bytes of that character.
	    if (has_mbyte)
	    {
		int i;
		int len = (*mb_ptr2len)(ptr) - 1;

		for (i = 0; i < len && n >= 1; ++i, --n)
		    *p++ = *ptr++;
	    }
	    *p++ = *ptr++;
	}
	*p = NUL;
    }

    /*
     * Execute the command.
     */
    if (cmdchar == '*' || cmdchar == '#')
    {
	if (!g_cmd && (has_mbyte
		    ? vim_iswordp(mb_prevptr(ml_get_curline(), ptr))
		    : vim_iswordc(ptr[-1])))
	    STRCAT(buf, "\\>");

	// put pattern in search history
	init_history();
	add_to_history(HIST_SEARCH, buf, TRUE, NUL);

	(void)normal_search(cap, cmdchar == '*' ? '/' : '?', buf, 0, NULL);
    }
    else
    {
	g_tag_at_cursor = TRUE;
	do_cmdline_cmd(buf);
	g_tag_at_cursor = FALSE;
    }

    vim_free(buf);
}