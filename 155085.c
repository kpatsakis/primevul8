win_free(
    win_T	*wp,
    tabpage_T	*tp)		/* tab page "win" is in, NULL for current */
{
    int		i;
    buf_T	*buf;
    wininfo_T	*wip;

#ifdef FEAT_FOLDING
    clearFolding(wp);
#endif

    /* reduce the reference count to the argument list. */
    alist_unlink(wp->w_alist);

    /* Don't execute autocommands while the window is halfway being deleted.
     * gui_mch_destroy_scrollbar() may trigger a FocusGained event. */
    block_autocmds();

#ifdef FEAT_LUA
    lua_window_free(wp);
#endif

#ifdef FEAT_MZSCHEME
    mzscheme_window_free(wp);
#endif

#ifdef FEAT_PERL
    perl_win_free(wp);
#endif

#ifdef FEAT_PYTHON
    python_window_free(wp);
#endif

#ifdef FEAT_PYTHON3
    python3_window_free(wp);
#endif

#ifdef FEAT_TCL
    tcl_window_free(wp);
#endif

#ifdef FEAT_RUBY
    ruby_window_free(wp);
#endif

    clear_winopt(&wp->w_onebuf_opt);
    clear_winopt(&wp->w_allbuf_opt);

#ifdef FEAT_EVAL
    vars_clear(&wp->w_vars->dv_hashtab);	/* free all w: variables */
    hash_init(&wp->w_vars->dv_hashtab);
    unref_var_dict(wp->w_vars);
#endif

    {
	tabpage_T	*ttp;

	if (prevwin == wp)
	    prevwin = NULL;
	FOR_ALL_TABPAGES(ttp)
	    if (ttp->tp_prevwin == wp)
		ttp->tp_prevwin = NULL;
    }
    win_free_lsize(wp);

    for (i = 0; i < wp->w_tagstacklen; ++i)
    {
	vim_free(wp->w_tagstack[i].tagname);
	vim_free(wp->w_tagstack[i].user_data);
    }
    vim_free(wp->w_localdir);

    /* Remove the window from the b_wininfo lists, it may happen that the
     * freed memory is re-used for another window. */
    FOR_ALL_BUFFERS(buf)
	for (wip = buf->b_wininfo; wip != NULL; wip = wip->wi_next)
	    if (wip->wi_win == wp)
		wip->wi_win = NULL;

#ifdef FEAT_SEARCH_EXTRA
    clear_matches(wp);
#endif

#ifdef FEAT_JUMPLIST
    free_jumplist(wp);
#endif

#ifdef FEAT_QUICKFIX
    qf_free_all(wp);
#endif

#ifdef FEAT_GUI
    if (gui.in_use)
    {
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_LEFT]);
	gui_mch_destroy_scrollbar(&wp->w_scrollbars[SBAR_RIGHT]);
    }
#endif /* FEAT_GUI */

#ifdef FEAT_MENU
    remove_winbar(wp);
#endif
#ifdef FEAT_TEXT_PROP
    free_callback(&wp->w_close_cb);
    free_callback(&wp->w_filter_cb);
    for (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    vim_free(wp->w_scrollbar_highlight);
    vim_free(wp->w_thumb_highlight);
    vim_free(wp->w_popup_title);
    list_unref(wp->w_popup_mask);
    vim_free(wp->w_popup_mask_cells);
#endif

#ifdef FEAT_SYN_HL
    vim_free(wp->w_p_cc_cols);
#endif

    if (win_valid_any_tab(wp))
	win_remove(wp, tp);
    if (autocmd_busy)
    {
	wp->w_next = au_pending_free_win;
	au_pending_free_win = wp;
    }
    else
	vim_free(wp);

    unblock_autocmds();
}