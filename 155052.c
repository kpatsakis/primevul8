enter_tabpage(
    tabpage_T	*tp,
    buf_T	*old_curbuf UNUSED,
    int		trigger_enter_autocmds,
    int		trigger_leave_autocmds)
{
    int		old_off = tp->tp_firstwin->w_winrow;
    win_T	*next_prevwin = tp->tp_prevwin;

    curtab = tp;
    firstwin = tp->tp_firstwin;
    lastwin = tp->tp_lastwin;
    topframe = tp->tp_topframe;

    /* We would like doing the TabEnter event first, but we don't have a
     * valid current window yet, which may break some commands.
     * This triggers autocommands, thus may make "tp" invalid. */
    win_enter_ext(tp->tp_curwin, FALSE, TRUE, FALSE,
			      trigger_enter_autocmds, trigger_leave_autocmds);
    prevwin = next_prevwin;

    last_status(FALSE);		/* status line may appear or disappear */
    (void)win_comp_pos();	/* recompute w_winrow for all windows */
#ifdef FEAT_DIFF
    diff_need_scrollbind = TRUE;
#endif

    /* The tabpage line may have appeared or disappeared, may need to resize
     * the frames for that.  When the Vim window was resized need to update
     * frame sizes too.  Use the stored value of p_ch, so that it can be
     * different for each tab page. */
    if (p_ch != curtab->tp_ch_used)
	clear_cmdline = TRUE;
    p_ch = curtab->tp_ch_used;
    if (curtab->tp_old_Rows != Rows || (old_off != firstwin->w_winrow
#ifdef FEAT_GUI_TABLINE
			    && !gui_use_tabline()
#endif
		))
	shell_new_rows();
    if (curtab->tp_old_Columns != Columns && starting == 0)
	shell_new_columns();	/* update window widths */

#if defined(FEAT_GUI)
    /* When 'guioptions' includes 'L' or 'R' may have to remove or add
     * scrollbars.  Have to update them anyway. */
    gui_may_update_scrollbars();
#endif

    /* Apply autocommands after updating the display, when 'rows' and
     * 'columns' have been set correctly. */
    if (trigger_enter_autocmds)
    {
	apply_autocmds(EVENT_TABENTER, NULL, NULL, FALSE, curbuf);
	if (old_curbuf != curbuf)
	    apply_autocmds(EVENT_BUFENTER, NULL, NULL, FALSE, curbuf);
    }

    redraw_all_later(NOT_VALID);
}