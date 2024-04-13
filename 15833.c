jumpto_tag(
    char_u	*lbuf_arg,	// line from the tags file for this tag
    int		forceit,	// :ta with !
    int		keep_help)	// keep help flag (FALSE for cscope)
{
    optmagic_T	save_magic_overruled;
    int		save_p_ws, save_p_scs, save_p_ic;
    linenr_T	save_lnum;
    char_u	*str;
    char_u	*pbuf;			// search pattern buffer
    char_u	*pbuf_end;
    char_u	*tofree_fname = NULL;
    char_u	*fname;
    tagptrs_T	tagp;
    int		retval = FAIL;
    int		getfile_result = GETFILE_UNUSED;
    int		search_options;
#ifdef FEAT_SEARCH_EXTRA
    int		save_no_hlsearch;
#endif
#if defined(FEAT_QUICKFIX)
    win_T	*curwin_save = NULL;
#endif
    char_u	*full_fname = NULL;
#ifdef FEAT_FOLDING
    int		old_KeyTyped = KeyTyped;    // getting the file may reset it
#endif
    size_t	len;
    char_u	*lbuf;

    // Make a copy of the line, it can become invalid when an autocommand calls
    // back here recursively.
    len = matching_line_len(lbuf_arg) + 1;
    lbuf = alloc(len);
    if (lbuf != NULL)
	mch_memmove(lbuf, lbuf_arg, len);

    pbuf = alloc(LSIZE);

    // parse the match line into the tagp structure
    if (pbuf == NULL || lbuf == NULL || parse_match(lbuf, &tagp) == FAIL)
    {
	tagp.fname_end = NULL;
	goto erret;
    }

    // truncate the file name, so it can be used as a string
    *tagp.fname_end = NUL;
    fname = tagp.fname;

    // copy the command to pbuf[], remove trailing CR/NL
    str = tagp.command;
    for (pbuf_end = pbuf; *str && *str != '\n' && *str != '\r'; )
    {
#ifdef FEAT_EMACS_TAGS
	if (tagp.is_etag && *str == ',')// stop at ',' after line number
	    break;
#endif
	*pbuf_end++ = *str++;
	if (pbuf_end - pbuf + 1 >= LSIZE)
	    break;
    }
    *pbuf_end = NUL;

#ifdef FEAT_EMACS_TAGS
    if (!tagp.is_etag)
#endif
    {
	/*
	 * Remove the "<Tab>fieldname:value" stuff; we don't need it here.
	 */
	str = pbuf;
	if (find_extra(&str) == OK)
	{
	    pbuf_end = str;
	    *pbuf_end = NUL;
	}
    }

    /*
     * Expand file name, when needed (for environment variables).
     * If 'tagrelative' option set, may change file name.
     */
    fname = expand_tag_fname(fname, tagp.tag_fname, TRUE);
    if (fname == NULL)
	goto erret;
    tofree_fname = fname;	// free() it later

    /*
     * Check if the file with the tag exists before abandoning the current
     * file.  Also accept a file name for which there is a matching BufReadCmd
     * autocommand event (e.g., http://sys/file).
     */
    if (mch_getperm(fname) < 0 && !has_autocmd(EVENT_BUFREADCMD, fname, NULL))
    {
	retval = NOTAGFILE;
	vim_free(nofile_fname);
	nofile_fname = vim_strsave(fname);
	if (nofile_fname == NULL)
	    nofile_fname = empty_option;
	goto erret;
    }

    ++RedrawingDisabled;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

#if defined(FEAT_QUICKFIX)
    if (g_do_tagpreview != 0)
    {
	postponed_split = 0;	// don't split again below
	curwin_save = curwin;	// Save current window

	/*
	 * If we are reusing a window, we may change dir when
	 * entering it (autocommands) so turn the tag filename
	 * into a fullpath
	 */
	if (!curwin->w_p_pvw)
	{
	    full_fname = FullName_save(fname, FALSE);
	    fname = full_fname;

	    /*
	     * Make the preview window the current window.
	     * Open a preview window when needed.
	     */
	    prepare_tagpreview(TRUE, TRUE, FALSE);
	}
    }

    // If it was a CTRL-W CTRL-] command split window now.  For ":tab tag"
    // open a new tab page.
    if (postponed_split && (swb_flags & (SWB_USEOPEN | SWB_USETAB)))
    {
	buf_T *existing_buf = buflist_findname_exp(fname);

	if (existing_buf != NULL)
	{
	    win_T *wp = NULL;

	    if (swb_flags & SWB_USEOPEN)
		wp = buf_jump_open_win(existing_buf);

	    // If 'switchbuf' contains "usetab": jump to first window in any tab
	    // page containing "existing_buf" if one exists
	    if (wp == NULL && (swb_flags & SWB_USETAB))
		wp = buf_jump_open_tab(existing_buf);
	    // We've switched to the buffer, the usual loading of the file must
	    // be skipped.
	    if (wp != NULL)
		getfile_result = GETFILE_SAME_FILE;
	}
    }
    if (getfile_result == GETFILE_UNUSED
				  && (postponed_split || cmdmod.cmod_tab != 0))
    {
	if (win_split(postponed_split > 0 ? postponed_split : 0,
						postponed_split_flags) == FAIL)
	{
	    --RedrawingDisabled;
	    goto erret;
	}
	RESET_BINDING(curwin);
    }
#endif

    if (keep_help)
    {
	// A :ta from a help file will keep the b_help flag set.  For ":ptag"
	// we need to use the flag from the window where we came from.
#if defined(FEAT_QUICKFIX)
	if (g_do_tagpreview != 0)
	    keep_help_flag = bt_help(curwin_save->w_buffer);
	else
#endif
	    keep_help_flag = curbuf->b_help;
    }

    if (getfile_result == GETFILE_UNUSED)
	// Careful: getfile() may trigger autocommands and call jumpto_tag()
	// recursively.
	getfile_result = getfile(0, fname, NULL, TRUE, (linenr_T)0, forceit);
    keep_help_flag = FALSE;

    if (GETFILE_SUCCESS(getfile_result))	// got to the right file
    {
	curwin->w_set_curswant = TRUE;
	postponed_split = 0;

	save_magic_overruled = magic_overruled;
	magic_overruled = OPTION_MAGIC_OFF;	// always execute with 'nomagic'
#ifdef FEAT_SEARCH_EXTRA
	// Save value of no_hlsearch, jumping to a tag is not a real search
	save_no_hlsearch = no_hlsearch;
#endif
#if defined(FEAT_PROP_POPUP) && defined(FEAT_QUICKFIX)
	// getfile() may have cleared options, apply 'previewpopup' again.
	if (g_do_tagpreview != 0 && *p_pvp != NUL)
	    parse_previewpopup(curwin);
#endif

	/*
	 * If 'cpoptions' contains 't', store the search pattern for the "n"
	 * command.  If 'cpoptions' does not contain 't', the search pattern
	 * is not stored.
	 */
	if (vim_strchr(p_cpo, CPO_TAGPAT) != NULL)
	    search_options = 0;
	else
	    search_options = SEARCH_KEEP;

	/*
	 * If the command is a search, try here.
	 *
	 * Reset 'smartcase' for the search, since the search pattern was not
	 * typed by the user.
	 * Only use do_search() when there is a full search command, without
	 * anything following.
	 */
	str = pbuf;
	if (pbuf[0] == '/' || pbuf[0] == '?')
	    str = skip_regexp(pbuf + 1, pbuf[0], FALSE) + 1;
	if (str > pbuf_end - 1)	// search command with nothing following
	{
	    save_p_ws = p_ws;
	    save_p_ic = p_ic;
	    save_p_scs = p_scs;
	    p_ws = TRUE;	// need 'wrapscan' for backward searches
	    p_ic = FALSE;	// don't ignore case now
	    p_scs = FALSE;
	    save_lnum = curwin->w_cursor.lnum;
	    if (tagp.tagline > 0)
		// start search before line from "line:" field
		curwin->w_cursor.lnum = tagp.tagline - 1;
	    else
		// start search before first line
		curwin->w_cursor.lnum = 0;
	    if (do_search(NULL, pbuf[0], pbuf[0], pbuf + 1, (long)1,
							 search_options, NULL))
		retval = OK;
	    else
	    {
		int	found = 1;
		int	cc;

		/*
		 * try again, ignore case now
		 */
		p_ic = TRUE;
		if (!do_search(NULL, pbuf[0], pbuf[0], pbuf + 1, (long)1,
							 search_options, NULL))
		{
		    /*
		     * Failed to find pattern, take a guess: "^func  ("
		     */
		    found = 2;
		    (void)test_for_static(&tagp);
		    cc = *tagp.tagname_end;
		    *tagp.tagname_end = NUL;
		    sprintf((char *)pbuf, "^%s\\s\\*(", tagp.tagname);
		    if (!do_search(NULL, '/', '/', pbuf, (long)1,
							 search_options, NULL))
		    {
			// Guess again: "^char * \<func  ("
			sprintf((char *)pbuf, "^\\[#a-zA-Z_]\\.\\*\\<%s\\s\\*(",
								tagp.tagname);
			if (!do_search(NULL, '/', '/', pbuf, (long)1,
							 search_options, NULL))
			    found = 0;
		    }
		    *tagp.tagname_end = cc;
		}
		if (found == 0)
		{
		    emsg(_(e_canot_find_tag_pattern));
		    curwin->w_cursor.lnum = save_lnum;
		}
		else
		{
		    /*
		     * Only give a message when really guessed, not when 'ic'
		     * is set and match found while ignoring case.
		     */
		    if (found == 2 || !save_p_ic)
		    {
			msg(_(e_couldnt_find_tag_just_guessing));
			if (!msg_scrolled && msg_silent == 0)
			{
			    out_flush();
			    ui_delay(1010L, TRUE);
			}
		    }
		    retval = OK;
		}
	    }
	    p_ws = save_p_ws;
	    p_ic = save_p_ic;
	    p_scs = save_p_scs;

	    // A search command may have positioned the cursor beyond the end
	    // of the line.  May need to correct that here.
	    check_cursor();
	}
	else
	{
	    int		save_secure = secure;

	    // Setup the sandbox for executing the command from the tags file.
	    secure = 1;
#ifdef HAVE_SANDBOX
	    ++sandbox;
#endif
	    curwin->w_cursor.lnum = 1;		// start command in line 1
	    do_cmdline_cmd(pbuf);
	    retval = OK;

	    // When the command has done something that is not allowed make
	    // sure the error message can be seen.
	    if (secure == 2)
		wait_return(TRUE);
	    secure = save_secure;
#ifdef HAVE_SANDBOX
	    --sandbox;
#endif
	}

	magic_overruled = save_magic_overruled;
#ifdef FEAT_SEARCH_EXTRA
	// restore no_hlsearch when keeping the old search pattern
	if (search_options)
	    set_no_hlsearch(save_no_hlsearch);
#endif

	// Return OK if jumped to another file (at least we found the file!).
	if (getfile_result == GETFILE_OPEN_OTHER)
	    retval = OK;

	if (retval == OK)
	{
	    /*
	     * For a help buffer: Put the cursor line at the top of the window,
	     * the help subject will be below it.
	     */
	    if (curbuf->b_help)
		set_topline(curwin, curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
	    if ((fdo_flags & FDO_TAG) && old_KeyTyped)
		foldOpenCursor();
#endif
	}

#if defined(FEAT_QUICKFIX)
	if (g_do_tagpreview != 0
			   && curwin != curwin_save && win_valid(curwin_save))
	{
	    // Return cursor to where we were
	    validate_cursor();
	    redraw_later(UPD_VALID);
	    win_enter(curwin_save, TRUE);
	}
#endif

	--RedrawingDisabled;
    }
    else
    {
	--RedrawingDisabled;
	got_int = FALSE;  // don't want entering window to fail

	if (postponed_split)		// close the window
	{
	    win_close(curwin, FALSE);
	    postponed_split = 0;
	}
#if defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
	else if (WIN_IS_POPUP(curwin))
	{
	    win_T   *wp = curwin;

	    if (win_valid(curwin_save))
		win_enter(curwin_save, TRUE);
	    popup_close(wp->w_id, FALSE);
	}
#endif
    }
#if defined(FEAT_QUICKFIX) && defined(FEAT_PROP_POPUP)
    if (WIN_IS_POPUP(curwin))
	// something went wrong, still in popup, but it can't have focus
	win_enter(firstwin, TRUE);
#endif

erret:
#if defined(FEAT_QUICKFIX)
    g_do_tagpreview = 0; // For next time
#endif
    vim_free(lbuf);
    vim_free(pbuf);
    vim_free(tofree_fname);
    vim_free(full_fname);

    return retval;
}