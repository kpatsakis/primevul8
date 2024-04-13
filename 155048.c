win_new_height(win_T *wp, int height)
{
    int		prev_height = wp->w_height;

    /* Don't want a negative height.  Happens when splitting a tiny window.
     * Will equalize heights soon to fix it. */
    if (height < 0)
	height = 0;
    if (wp->w_height == height)
	return;	    /* nothing to do */

    if (wp->w_height > 0)
    {
	if (wp == curwin)
	    /* w_wrow needs to be valid. When setting 'laststatus' this may
	     * call win_new_height() recursively. */
	    validate_cursor();
	if (wp->w_height != prev_height)
	    return;  /* Recursive call already changed the size, bail out here
			to avoid the following to mess things up. */
	if (wp->w_wrow != wp->w_prev_fraction_row)
	    set_fraction(wp);
    }

    wp->w_height = height;
    wp->w_skipcol = 0;

    /* There is no point in adjusting the scroll position when exiting.  Some
     * values might be invalid. */
    if (!exiting)
	scroll_to_fraction(wp, prev_height);
}