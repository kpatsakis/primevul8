last_status(
    int		morewin)	/* pretend there are two or more windows */
{
    /* Don't make a difference between horizontal or vertical split. */
    last_status_rec(topframe, (p_ls == 2
			  || (p_ls == 1 && (morewin || !ONE_WINDOW))));
}