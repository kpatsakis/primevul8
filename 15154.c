set_context_for_expression(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    int		has_expr = cmdidx != CMD_let && cmdidx != CMD_var;
    int		c;
    char_u	*p;

    if (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	if (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == NULL)
	{
	    // ":let var1 var2 ...": find last space.
	    for (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_pattern = p;
		MB_PTR_BACK(arg, p);
		if (VIM_ISWHITE(*p))
		    break;
	    }
	    return;
	}
    }
    else
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_EXPRESSION;
    while ((xp->xp_pattern = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != NULL)
    {
	c = *xp->xp_pattern;
	if (c == '&')
	{
	    c = xp->xp_pattern[1];
	    if (c == '&')
	    {
		++xp->xp_pattern;
		xp->xp_context = has_expr ? EXPAND_EXPRESSION : EXPAND_NOTHING;
	    }
	    else if (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		if ((c == 'l' || c == 'g') && xp->xp_pattern[2] == ':')
		    xp->xp_pattern += 2;

	    }
	}
	else if (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	else if (c == '=')
	{
	    has_expr = TRUE;
	    xp->xp_context = EXPAND_EXPRESSION;
	}
	else if (c == '#'
		&& xp->xp_context == EXPAND_EXPRESSION)
	{
	    // Autoload function/variable contains '#'.
	    break;
	}
	else if ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_pattern, '(') == NULL)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    break;
	}
	else if (has_expr)
	{
	    if (c == '"')	    // string
	    {
		while ((c = *++xp->xp_pattern) != NUL && c != '"')
		    if (c == '\\' && xp->xp_pattern[1] != NUL)
			++xp->xp_pattern;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    else if (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		while ((c = *++xp->xp_pattern) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    else if (c == '|')
	    {
		if (xp->xp_pattern[1] == '|')
		{
		    ++xp->xp_pattern;
		    xp->xp_context = EXPAND_EXPRESSION;
		}
		else
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    else
		xp->xp_context = EXPAND_EXPRESSION;
	}
	else
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_EXPRESSION;
	arg = xp->xp_pattern;
	if (*arg != NUL)
	    while ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    if ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg)
	    && xp->xp_context == EXPAND_EXPRESSION)
    {
	for (;;)
	{
	    char_u *n = skiptowhite(arg);

	    if (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		break;
	    arg = skipwhite(n);
	}
    }

    xp->xp_pattern = arg;
}