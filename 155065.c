leave_tabpage(
    buf_T	*new_curbuf UNUSED,    /* what is going to be the new curbuf,
				       NULL if unknown */
    int		trigger_leave_autocmds UNUSED)
{
    tabpage_T	*tp = curtab;

#ifdef FEAT_JOB_CHANNEL
    leaving_window(curwin);
#endif
    reset_VIsual_and_resel();	/* stop Visual mode */
    if (trigger_leave_autocmds)
    {
	if (new_curbuf != curbuf)
	{
	    apply_autocmds(EVENT_BUFLEAVE, NULL, NULL, FALSE, curbuf);
	    if (curtab != tp)
		return FAIL;
	}
	apply_autocmds(EVENT_WINLEAVE, NULL, NULL, FALSE, curbuf);
	if (curtab != tp)
	    return FAIL;
	apply_autocmds(EVENT_TABLEAVE, NULL, NULL, FALSE, curbuf);
	if (curtab != tp)
	    return FAIL;
    }
#if defined(FEAT_GUI)
    /* Remove the scrollbars.  They may be added back later. */
    if (gui.in_use)
	gui_remove_scrollbars();
#endif
    tp->tp_curwin = curwin;
    tp->tp_prevwin = prevwin;
    tp->tp_firstwin = firstwin;
    tp->tp_lastwin = lastwin;
    tp->tp_old_Rows = Rows;
    tp->tp_old_Columns = Columns;
    firstwin = NULL;
    lastwin = NULL;
    return OK;
}