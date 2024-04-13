find_tags(
    char_u	*pat,			// pattern to search for
    int		*num_matches,		// return: number of matches found
    char_u	***matchesp,		// return: array of matches found
    int		flags,
    int		mincount,		// MAXCOL: find all matches
					// other: minimal number of matches
    char_u	*buf_ffname)		// name of buffer for priority
{
    findtags_state_T	st;
    tagname_T	tn;			// info for get_tagfname()
    int		first_file;		// trying first tag file
    int		retval = FAIL;		// return value
    int		round;

    int		save_emsg_off;

    int		help_save;
#ifdef FEAT_MULTI_LANG
    int		i;
    char_u	*saved_pat = NULL;		// copy of pat[]
#endif

    int		findall = (mincount == MAXCOL || mincount == TAG_MANY);
						// find all matching tags
    int		has_re = (flags & TAG_REGEXP);	// regexp used
    int		noic = (flags & TAG_NOIC);
#ifdef FEAT_CSCOPE
    int		use_cscope = (flags & TAG_CSCOPE);
#endif
    int		verbose = (flags & TAG_VERBOSE);
    int		save_p_ic = p_ic;

    /*
     * Change the value of 'ignorecase' according to 'tagcase' for the
     * duration of this function.
     */
    switch (curbuf->b_tc_flags ? curbuf->b_tc_flags : tc_flags)
    {
	case TC_FOLLOWIC:		 break;
	case TC_IGNORE:    p_ic = TRUE;  break;
	case TC_MATCH:     p_ic = FALSE; break;
	case TC_FOLLOWSCS: p_ic = ignorecase(pat); break;
	case TC_SMART:     p_ic = ignorecase_opt(pat, TRUE, TRUE); break;
    }

    help_save = curbuf->b_help;

    if (findtags_state_init(&st, pat, flags, mincount) == FAIL)
	goto findtag_end;

#ifdef FEAT_CSCOPE
    STRCPY(st.tag_fname, "from cscope");	// for error messages
#endif

    /*
     * Initialize a few variables
     */
    if (st.help_only)				// want tags from help file
	curbuf->b_help = TRUE;			// will be restored later
#ifdef FEAT_CSCOPE
    else if (use_cscope)
    {
	// Make sure we don't mix help and cscope, confuses Coverity.
	st.help_only = FALSE;
	curbuf->b_help = FALSE;
    }
#endif

#ifdef FEAT_MULTI_LANG
    if (curbuf->b_help)
    {
	// When "@ab" is specified use only the "ab" language, otherwise
	// search all languages.
	if (st.orgpat->len > 3 && pat[st.orgpat->len - 3] == '@'
				&& ASCII_ISALPHA(pat[st.orgpat->len - 2])
				&& ASCII_ISALPHA(pat[st.orgpat->len - 1]))
	{
	    saved_pat = vim_strnsave(pat, st.orgpat->len - 3);
	    if (saved_pat != NULL)
	    {
		st.help_lang_find = &pat[st.orgpat->len - 2];
		st.orgpat->pat = saved_pat;
		st.orgpat->len -= 3;
	    }
	}
    }
#endif
    if (p_tl != 0 && st.orgpat->len > p_tl)	// adjust for 'taglength'
	st.orgpat->len = p_tl;

    save_emsg_off = emsg_off;
    emsg_off = TRUE;  // don't want error for invalid RE here
    prepare_pats(st.orgpat, has_re);
    emsg_off = save_emsg_off;
    if (has_re && st.orgpat->regmatch.regprog == NULL)
	goto findtag_end;

#ifdef FEAT_EVAL
    retval = findtags_apply_tfu(&st, pat, buf_ffname);
    if (retval != NOTDONE)
	goto findtag_end;

    // re-initialize the default return value
    retval = FAIL;
#endif

#ifdef FEAT_MULTI_LANG
    // Set a flag if the file extension is .txt
    if ((flags & TAG_KEEP_LANG)
	    && st.help_lang_find == NULL
	    && curbuf->b_fname != NULL
	    && (i = (int)STRLEN(curbuf->b_fname)) > 4
	    && STRICMP(curbuf->b_fname + i - 4, ".txt") == 0)
	st.is_txt = TRUE;
#endif

    /*
     * When finding a specified number of matches, first try with matching
     * case, so binary search can be used, and try ignore-case matches in a
     * second loop.
     * When finding all matches, 'tagbsearch' is off, or there is no fixed
     * string to look for, ignore case right away to avoid going though the
     * tags files twice.
     * When the tag file is case-fold sorted, it is either one or the other.
     * Only ignore case when TAG_NOIC not used or 'ignorecase' set.
     */
    st.orgpat->regmatch.rm_ic = ((p_ic || !noic)
			&& (findall || st.orgpat->headlen == 0 || !p_tbs));
    for (round = 1; round <= 2; ++round)
    {
	st.linear = (st.orgpat->headlen == 0 || !p_tbs || round == 2);

      /*
       * Try tag file names from tags option one by one.
       */
      for (first_file = TRUE;
#ifdef FEAT_CSCOPE
	    use_cscope ||
#endif
		get_tagfname(&tn, first_file, st.tag_fname) == OK;
							   first_file = FALSE)
      {
	  findtags_in_file(&st, buf_ffname);
	  if (st.stop_searching
#ifdef FEAT_CSCOPE
		  || use_cscope
#endif
	     )
	  {
	      retval = OK;
	      break;
	  }
      } // end of for-each-file loop

#ifdef FEAT_CSCOPE
	if (!use_cscope)
#endif
	    tagname_free(&tn);

	// stop searching when already did a linear search, or when TAG_NOIC
	// used, and 'ignorecase' not set or already did case-ignore search
	if (st.stop_searching || st.linear || (!p_ic && noic) ||
						st.orgpat->regmatch.rm_ic)
	    break;
# ifdef FEAT_CSCOPE
	if (use_cscope)
	    break;
# endif

	// try another time while ignoring case
	st.orgpat->regmatch.rm_ic = TRUE;
    }

    if (!st.stop_searching)
    {
	if (!st.did_open && verbose)	// never opened any tags file
	    emsg(_(e_no_tags_file));
	retval = OK;		// It's OK even when no tag found
    }

findtag_end:
    findtags_state_free(&st);

    /*
     * Move the matches from the ga_match[] arrays into one list of
     * matches.  When retval == FAIL, free the matches.
     */
    if (retval == FAIL)
	st.match_count = 0;

    *num_matches = findtags_copy_matches(&st, matchesp);

    curbuf->b_help = help_save;
#ifdef FEAT_MULTI_LANG
    vim_free(saved_pat);
#endif

    p_ic = save_p_ic;

    return retval;
}