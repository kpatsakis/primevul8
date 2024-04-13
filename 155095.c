entering_window(win_T *win)
{
    // Only matters for a prompt window.
    if (!bt_prompt(win->w_buffer))
	return;

    // When switching to a prompt buffer that was in Insert mode, don't stop
    // Insert mode, it may have been set in leaving_window().
    if (win->w_buffer->b_prompt_insert != NUL)
	stop_insert_mode = FALSE;

    // When entering the prompt window restart Insert mode if we were in Insert
    // mode when we left it.
    restart_edit = win->w_buffer->b_prompt_insert;
}