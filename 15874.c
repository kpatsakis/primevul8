emacs_tags_parse_line(char_u *lbuf, tagptrs_T *tagp)
{
    char_u	*p_7f;
    char_u	*p;

    // There are two formats for an emacs tag line:
    // 1:  struct EnvBase ^?EnvBase^A139,4627
    // 2: #define	ARPB_WILD_WORLD ^?153,5194
    p_7f = vim_strchr(lbuf, 0x7f);
    if (p_7f == NULL)
    {
etag_fail:
	if (vim_strchr(lbuf, '\n') != NULL)
	    return FAIL;

	// Truncated line.  Ignore it.
	if (p_verbose >= 5)
	{
	    verbose_enter();
	    msg(_("Ignoring long line in tags file"));
	    verbose_leave();
	}
	tagp->command = lbuf;
	tagp->tagname = lbuf;
	tagp->tagname_end = lbuf;
	return OK;
    }

    // Find ^A.  If not found the line number is after the 0x7f
    p = vim_strchr(p_7f, Ctrl_A);
    if (p == NULL)
	p = p_7f + 1;
    else
	++p;

    if (!VIM_ISDIGIT(*p))	    // check for start of line number
	goto etag_fail;
    tagp->command = p;

    if (p[-1] == Ctrl_A)	    // first format: explicit tagname given
    {
	tagp->tagname = p_7f + 1;
	tagp->tagname_end = p - 1;
    }
    else			    // second format: isolate tagname
    {
	// find end of tagname
	for (p = p_7f - 1; !vim_iswordc(*p); --p)
	    if (p == lbuf)
		goto etag_fail;
	tagp->tagname_end = p + 1;
	while (p >= lbuf && vim_iswordc(*p))
	    --p;
	tagp->tagname = p + 1;
    }

    return OK;
}