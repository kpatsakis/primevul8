do_insert_char_pre(int c)
{
    char_u	*res;
    char_u	buf[MB_MAXBYTES + 1];
    int		save_State = State;

    // Return quickly when there is nothing to do.
    if (!has_insertcharpre())
	return NULL;

    if (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    else
    {
	buf[0] = c;
	buf[1] = NUL;
    }

    // Lock the text to avoid weird things from happening.
    ++textlock;
    set_vim_var_string(VV_CHAR, buf, -1);  // set v:char

    res = NULL;
    if (ins_apply_autocmds(EVENT_INSERTCHARPRE))
    {
	// Get the value of v:char.  It may be empty or more than one
	// character.  Only use it when changed, otherwise continue with the
	// original character to avoid breaking autoindent.
	if (STRCMP(buf, get_vim_var_str(VV_CHAR)) != 0)
	    res = vim_strsave(get_vim_var_str(VV_CHAR));
    }

    set_vim_var_string(VV_CHAR, NULL, -1);  // clear v:char
    --textlock;

    // Restore the State, it may have been changed.
    State = save_State;

    return res;
}