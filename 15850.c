print_tag_list(
    int		new_tag,
    int		use_tagstack,
    int		num_matches,
    char_u	**matches)
{
    taggy_T	*tagstack = curwin->w_tagstack;
    int		tagstackidx = curwin->w_tagstackidx;
    int		i;
    char_u	*p;
    char_u	*command_end;
    tagptrs_T	tagp;
    int		taglen;
    int		attr;

    /*
     * Assume that the first match indicates how long the tags can
     * be, and align the file names to that.
     */
    parse_match(matches[0], &tagp);
    taglen = (int)(tagp.tagname_end - tagp.tagname + 2);
    if (taglen < 18)
	taglen = 18;
    if (taglen > Columns - 25)
	taglen = MAXCOL;
    if (msg_col == 0)
	msg_didout = FALSE;	// overwrite previous message
    msg_start();
    msg_puts_attr(_("  # pri kind tag"), HL_ATTR(HLF_T));
    msg_clr_eos();
    taglen_advance(taglen);
    msg_puts_attr(_("file\n"), HL_ATTR(HLF_T));

    for (i = 0; i < num_matches && !got_int; ++i)
    {
	parse_match(matches[i], &tagp);
	if (!new_tag && (
#if defined(FEAT_QUICKFIX)
		    (g_do_tagpreview != 0
		     && i == ptag_entry.cur_match) ||
#endif
		    (use_tagstack
		     && i == tagstack[tagstackidx].cur_match)))
	    *IObuff = '>';
	else
	    *IObuff = ' ';
	vim_snprintf((char *)IObuff + 1, IOSIZE - 1,
		"%2d %s ", i + 1,
			       mt_names[matches[i][0] & MT_MASK]);
	msg_puts((char *)IObuff);
	if (tagp.tagkind != NULL)
	    msg_outtrans_len(tagp.tagkind,
			  (int)(tagp.tagkind_end - tagp.tagkind));
	msg_advance(13);
	msg_outtrans_len_attr(tagp.tagname,
			   (int)(tagp.tagname_end - tagp.tagname),
						  HL_ATTR(HLF_T));
	msg_putchar(' ');
	taglen_advance(taglen);

	// Find out the actual file name. If it is long, truncate
	// it and put "..." in the middle
	p = tag_full_fname(&tagp);
	if (p != NULL)
	{
	    msg_outtrans_long_attr(p, HL_ATTR(HLF_D));
	    vim_free(p);
	}
	if (msg_col > 0)
	    msg_putchar('\n');
	if (got_int)
	    break;
	msg_advance(15);

	// print any extra fields
	command_end = tagp.command_end;
	if (command_end != NULL)
	{
	    p = command_end + 3;
	    while (*p && *p != '\r' && *p != '\n')
	    {
		while (*p == TAB)
		    ++p;

		// skip "file:" without a value (static tag)
		if (STRNCMP(p, "file:", 5) == 0
					     && vim_isspace(p[5]))
		{
		    p += 5;
		    continue;
		}
		// skip "kind:<kind>" and "<kind>"
		if (p == tagp.tagkind
			|| (p + 5 == tagp.tagkind
				&& STRNCMP(p, "kind:", 5) == 0))
		{
		    p = tagp.tagkind_end;
		    continue;
		}
		// print all other extra fields
		attr = HL_ATTR(HLF_CM);
		while (*p && *p != '\r' && *p != '\n')
		{
		    if (msg_col + ptr2cells(p) >= Columns)
		    {
			msg_putchar('\n');
			if (got_int)
			    break;
			msg_advance(15);
		    }
		    p = msg_outtrans_one(p, attr);
		    if (*p == TAB)
		    {
			msg_puts_attr(" ", attr);
			break;
		    }
		    if (*p == ':')
			attr = 0;
		}
	    }
	    if (msg_col > 15)
	    {
		msg_putchar('\n');
		if (got_int)
		    break;
		msg_advance(15);
	    }
	}
	else
	{
	    for (p = tagp.command;
			      *p && *p != '\r' && *p != '\n'; ++p)
		;
	    command_end = p;
	}

	// Put the info (in several lines) at column 15.
	// Don't display "/^" and "?^".
	p = tagp.command;
	if (*p == '/' || *p == '?')
	{
	    ++p;
	    if (*p == '^')
		++p;
	}
	// Remove leading whitespace from pattern
	while (p != command_end && vim_isspace(*p))
	    ++p;

	while (p != command_end)
	{
	    if (msg_col + (*p == TAB ? 1 : ptr2cells(p)) > Columns)
		msg_putchar('\n');
	    if (got_int)
		break;
	    msg_advance(15);

	    // skip backslash used for escaping a command char or
	    // a backslash
	    if (*p == '\\' && (*(p + 1) == *tagp.command
			    || *(p + 1) == '\\'))
		++p;

	    if (*p == TAB)
	    {
		msg_putchar(' ');
		++p;
	    }
	    else
		p = msg_outtrans_one(p, 0);

	    // don't display the "$/;\"" and "$?;\""
	    if (p == command_end - 2 && *p == '$'
				     && *(p + 1) == *tagp.command)
		break;
	    // don't display matching '/' or '?'
	    if (p == command_end - 1 && *p == *tagp.command
				     && (*p == '/' || *p == '?'))
		break;
	}
	if (msg_col)
	    msg_putchar('\n');
	ui_breakcheck();
    }
    if (got_int)
	got_int = FALSE;	// only stop the listing
}