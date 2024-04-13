get_tagfname(
    tagname_T	*tnp,	// holds status info
    int		first,	// TRUE when first file name is wanted
    char_u	*buf)	// pointer to buffer of MAXPATHL chars
{
    char_u		*fname = NULL;
    char_u		*r_ptr;
    int			i;

    if (first)
	CLEAR_POINTER(tnp);

    if (curbuf->b_help)
    {
	/*
	 * For help files it's done in a completely different way:
	 * Find "doc/tags" and "doc/tags-??" in all directories in
	 * 'runtimepath'.
	 */
	if (first)
	{
	    ga_clear_strings(&tag_fnames);
	    ga_init2(&tag_fnames, sizeof(char_u *), 10);
	    do_in_runtimepath((char_u *)
#ifdef FEAT_MULTI_LANG
# ifdef VMS
		    // Functions decc$to_vms() and decc$translate_vms() crash
		    // on some VMS systems with wildcards "??".  Seems ECO
		    // patches do fix the problem in C RTL, but we can't use
		    // an #ifdef for that.
		    "doc/tags doc/tags-*"
# else
		    "doc/tags doc/tags-??"
# endif
#else
		    "doc/tags"
#endif
					   , DIP_ALL, found_tagfile_cb, NULL);
	}

	if (tnp->tn_hf_idx >= tag_fnames.ga_len)
	{
	    // Not found in 'runtimepath', use 'helpfile', if it exists and
	    // wasn't used yet, replacing "help.txt" with "tags".
	    if (tnp->tn_hf_idx > tag_fnames.ga_len || *p_hf == NUL)
		return FAIL;
	    ++tnp->tn_hf_idx;
	    STRCPY(buf, p_hf);
	    STRCPY(gettail(buf), "tags");
#ifdef BACKSLASH_IN_FILENAME
	    slash_adjust(buf);
#endif
	    simplify_filename(buf);

	    for (i = 0; i < tag_fnames.ga_len; ++i)
		if (STRCMP(buf, ((char_u **)(tag_fnames.ga_data))[i]) == 0)
		    return FAIL; // avoid duplicate file names
	}
	else
	    vim_strncpy(buf, ((char_u **)(tag_fnames.ga_data))[
					     tnp->tn_hf_idx++], MAXPATHL - 1);
	return OK;
    }

    if (first)
    {
	// Init.  We make a copy of 'tags', because autocommands may change
	// the value without notifying us.
	tnp->tn_tags = vim_strsave((*curbuf->b_p_tags != NUL)
						 ? curbuf->b_p_tags : p_tags);
	if (tnp->tn_tags == NULL)
	    return FAIL;
	tnp->tn_np = tnp->tn_tags;
    }

    /*
     * Loop until we have found a file name that can be used.
     * There are two states:
     * tnp->tn_did_filefind_init == FALSE: setup for next part in 'tags'.
     * tnp->tn_did_filefind_init == TRUE: find next file in this part.
     */
    for (;;)
    {
	if (tnp->tn_did_filefind_init)
	{
	    fname = vim_findfile(tnp->tn_search_ctx);
	    if (fname != NULL)
		break;

	    tnp->tn_did_filefind_init = FALSE;
	}
	else
	{
	    char_u  *filename = NULL;

	    // Stop when used all parts of 'tags'.
	    if (*tnp->tn_np == NUL)
	    {
		vim_findfile_cleanup(tnp->tn_search_ctx);
		tnp->tn_search_ctx = NULL;
		return FAIL;
	    }

	    /*
	     * Copy next file name into buf.
	     */
	    buf[0] = NUL;
	    (void)copy_option_part(&tnp->tn_np, buf, MAXPATHL - 1, " ,");

#ifdef FEAT_PATH_EXTRA
	    r_ptr = vim_findfile_stopdir(buf);
#else
	    r_ptr = NULL;
#endif
	    // move the filename one char forward and truncate the
	    // filepath with a NUL
	    filename = gettail(buf);
	    STRMOVE(filename + 1, filename);
	    *filename++ = NUL;

	    tnp->tn_search_ctx = vim_findfile_init(buf, filename,
		    r_ptr, 100,
		    FALSE,	   // don't free visited list
		    FINDFILE_FILE, // we search for a file
		    tnp->tn_search_ctx, TRUE, curbuf->b_ffname);
	    if (tnp->tn_search_ctx != NULL)
		tnp->tn_did_filefind_init = TRUE;
	}
    }

    STRCPY(buf, fname);
    vim_free(fname);
    return OK;
}