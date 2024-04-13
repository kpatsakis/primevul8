win_free_popup(win_T *win)
{
    if (bt_popup(win->w_buffer))
	win_close_buffer(win, DOBUF_WIPE_REUSE, FALSE);
    else
	close_buffer(win, win->w_buffer, 0, FALSE);
# if defined(FEAT_TIMERS)
    if (win->w_popup_timer != NULL)
	stop_timer(win->w_popup_timer);
# endif
    vim_free(win->w_frame);
    win_free(win, NULL);
}