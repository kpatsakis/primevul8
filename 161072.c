check_auto_format(
    int		end_insert)	    // TRUE when ending Insert mode
{
    int		c = ' ';
    int		cc;

    if (did_add_space)
    {
	cc = gchar_cursor();
	if (!WHITECHAR(cc))
	    // Somehow the space was removed already.
	    did_add_space = FALSE;
	else
	{
	    if (!end_insert)
	    {
		inc_cursor();
		c = gchar_cursor();
		dec_cursor();
	    }
	    if (c != NUL)
	    {
		// The space is no longer at the end of the line, delete it.
		del_char(FALSE);
		did_add_space = FALSE;
	    }
	}
    }
}