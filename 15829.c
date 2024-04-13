findtags_in_file(findtags_state_T *st, char_u *buf_ffname)
{
    findtags_match_args_T margs;
#ifdef FEAT_CSCOPE
    int		use_cscope = (st->flags & TAG_CSCOPE);
#endif

    st->vimconv.vc_type = CONV_NONE;
    st->tag_file_sorted = NUL;
    st->fp = NULL;
    findtags_matchargs_init(&margs, st->flags);

    // A file that doesn't exist is silently ignored.  Only when not a
    // single file is found, an error message is given (further on).
#ifdef FEAT_CSCOPE
    if (use_cscope)
	st->fp = NULL;	    // avoid GCC warning
    else
#endif
    {
#ifdef FEAT_MULTI_LANG
	if (curbuf->b_help)
	{
	    if (!findtags_in_help_init(st))
		return;
	}
#endif

	st->fp = mch_fopen((char *)st->tag_fname, "r");
	if (st->fp == NULL)
	    return;

	if (p_verbose >= 5)
	{
	    verbose_enter();
	    smsg(_("Searching tags file %s"), st->tag_fname);
	    verbose_leave();
	}
    }
    st->did_open = TRUE;	// remember that we found at least one file

    st->state = TS_START;	// we're at the start of the file
#ifdef FEAT_EMACS_TAGS
    st->is_etag = FALSE;	// default is: not emacs style
#endif

    // Read and parse the lines in the file one by one
    findtags_get_all_tags(st, &margs, buf_ffname);

    if (st->fp != NULL)
    {
	fclose(st->fp);
	st->fp = NULL;
    }
#ifdef FEAT_EMACS_TAGS
    emacs_tags_incstack_free();
#endif
    if (st->vimconv.vc_type != CONV_NONE)
	convert_setup(&st->vimconv, NULL, NULL);

    if (margs.sort_error)
	semsg(_(e_tags_file_not_sorted_str), st->tag_fname);

    // Stop searching if sufficient tags have been found.
    if (st->match_count >= st->mincount)
	st->stop_searching = TRUE;
}