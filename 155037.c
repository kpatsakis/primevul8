leaving_window(win_T *win)
{
    // Only matters for a prompt window.
    if (!bt_prompt(win->w_buffer))
	return;

    // When leaving a prompt window stop Insert mode and perhaps restart
    // it when entering that window again.
    win->w_buffer->b_prompt_insert = restart_edit;
    if (restart_edit != 0 && mode_displayed)
	clear_cmdline = TRUE;		/* unshow mode later */
    restart_edit = NUL;

    // When leaving the window (or closing the window) was done from a
    // callback we need to break out of the Insert mode loop and restart Insert
    // mode when entering the window again.
    if (State & INSERT)
    {
	stop_insert_mode = TRUE;
	if (win->w_buffer->b_prompt_insert == NUL)
	    win->w_buffer->b_prompt_insert = 'A';
    }
}