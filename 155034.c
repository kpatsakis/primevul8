win_goto_ver(
    int		up,		// TRUE to go to win above
    long	count)
{
    win_T	*win;

    win = win_vert_neighbor(curtab, curwin, up, count);
    if (win != NULL)
	win_goto(win);
}