expand_tag_fname(char_u *fname, char_u *tag_fname, int expand)
{
    char_u	*p;
    char_u	*retval;
    char_u	*expanded_fname = NULL;
    expand_T	xpc;

    /*
     * Expand file name (for environment variables) when needed.
     */
    if (expand && mch_has_wildcard(fname))
    {
	ExpandInit(&xpc);
	xpc.xp_context = EXPAND_FILES;
	expanded_fname = ExpandOne(&xpc, fname, NULL,
			    WILD_LIST_NOTFOUND|WILD_SILENT, WILD_EXPAND_FREE);
	if (expanded_fname != NULL)
	    fname = expanded_fname;
    }

    if ((p_tr || curbuf->b_help)
	    && !vim_isAbsName(fname)
	    && (p = gettail(tag_fname)) != tag_fname)
    {
	retval = alloc(MAXPATHL);
	if (retval != NULL)
	{
	    STRCPY(retval, tag_fname);
	    vim_strncpy(retval + (p - tag_fname), fname,
					      MAXPATHL - (p - tag_fname) - 1);
	    /*
	     * Translate names like "src/a/../b/file.c" into "src/b/file.c".
	     */
	    simplify_filename(retval);
	}
    }
    else
	retval = vim_strsave(fname);

    vim_free(expanded_fname);

    return retval;
}