win_free_lsize(win_T *wp)
{
    /* TODO: why would wp be NULL here? */
    if (wp != NULL)
	VIM_CLEAR(wp->w_lines);
}