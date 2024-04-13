parse_match(
    char_u	*lbuf,	    // input: matching line
    tagptrs_T	*tagp)	    // output: pointers into the line
{
    int		retval;
    char_u	*p;
    char_u	*pc, *pt;

    tagp->tag_fname = lbuf + 1;
    lbuf += STRLEN(tagp->tag_fname) + 2;
#ifdef FEAT_EMACS_TAGS
    if (*lbuf)
    {
	tagp->is_etag = TRUE;
	tagp->fname = lbuf;
	lbuf += STRLEN(lbuf);
	tagp->fname_end = lbuf++;
    }
    else
    {
	tagp->is_etag = FALSE;
	++lbuf;
    }
#endif

    // Find search pattern and the file name for non-etags.
    retval = parse_tag_line(lbuf,
#ifdef FEAT_EMACS_TAGS
			tagp->is_etag,
#endif
			tagp);

    tagp->tagkind = NULL;
    tagp->user_data = NULL;
    tagp->tagline = 0;
    tagp->command_end = NULL;

    if (retval == OK)
    {
	// Try to find a kind field: "kind:<kind>" or just "<kind>"
	p = tagp->command;
	if (find_extra(&p) == OK)
	{
	    if (p > tagp->command && p[-1] == '|')
		tagp->command_end = p - 1;  // drop trailing bar
	    else
		tagp->command_end = p;
	    p += 2;	// skip ";\""
	    if (*p++ == TAB)
		// Accept ASCII alphabetic kind characters and any multi-byte
		// character.
		while (ASCII_ISALPHA(*p) || mb_ptr2len(p) > 1)
		{
		    if (STRNCMP(p, "kind:", 5) == 0)
			tagp->tagkind = p + 5;
		    else if (STRNCMP(p, "user_data:", 10) == 0)
			tagp->user_data = p + 10;
		    else if (STRNCMP(p, "line:", 5) == 0)
			tagp->tagline = atoi((char *)p + 5);
		    if (tagp->tagkind != NULL && tagp->user_data != NULL)
			break;
		    pc = vim_strchr(p, ':');
		    pt = vim_strchr(p, '\t');
		    if (pc == NULL || (pt != NULL && pc > pt))
			tagp->tagkind = p;
		    if (pt == NULL)
			break;
		    p = pt;
		    MB_PTR_ADV(p);
		}
	}
	if (tagp->tagkind != NULL)
	{
	    for (p = tagp->tagkind;
			    *p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
		;
	    tagp->tagkind_end = p;
	}
	if (tagp->user_data != NULL)
	{
	    for (p = tagp->user_data;
			    *p && *p != '\t' && *p != '\r' && *p != '\n'; MB_PTR_ADV(p))
		;
	    tagp->user_data_end = p;
	}
    }
    return retval;
}