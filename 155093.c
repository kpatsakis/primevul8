win_equal(
    win_T	*next_curwin,	/* pointer to current window to be or NULL */
    int		current,	/* do only frame with current window */
    int		dir)		/* 'v' for vertically, 'h' for horizontally,
				   'b' for both, 0 for using p_ead */
{
    if (dir == 0)
	dir = *p_ead;
    win_equal_rec(next_curwin == NULL ? curwin : next_curwin, current,
		      topframe, dir, 0, tabline_height(),
					   (int)Columns, topframe->fr_height);
}