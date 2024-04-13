find_ex_command(
	exarg_T *eap,
	int	*full UNUSED,
	int	(*lookup)(char_u *, size_t, int cmd, cctx_T *) UNUSED,
	cctx_T	*cctx UNUSED)
{
    int		len;
    char_u	*p;
    int		i;
#ifndef FEAT_EVAL
    int		vim9 = FALSE;
#else
    int		vim9 = in_vim9script();

    /*
     * Recognize a Vim9 script function/method call and assignment:
     * "lvar = value", "lvar(arg)", "[1, 2 3]->Func()"
     */
    p = eap->cmd;
    if (lookup != NULL)
    {
	char_u *pskip = skip_option_env_lead(eap->cmd);

	if (vim_strchr((char_u *)"{('[\"@&$", *p) != NULL
	       || ((p = to_name_const_end(pskip)) > eap->cmd && *p != NUL)
	       || (p[0] == '0' && p[1] == 'z'))
	{
	    int	    oplen;
	    int	    heredoc;
	    char_u  *swp;

	    if (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || (eap->cmd[0] == '@'
					&& (valid_yank_reg(eap->cmd[1], FALSE)
						       || eap->cmd[1] == '@')))
	    {
		if (*eap->cmd == '&')
		{
		    p = eap->cmd + 1;
		    if (STRNCMP("l:", p, 2) == 0 || STRNCMP("g:", p, 2) == 0)
			p += 2;
		    p = to_name_end(p, FALSE);
		}
		else if (*eap->cmd == '$')
		    p = to_name_end(eap->cmd + 1, FALSE);
		else
		    p = eap->cmd + 2;
		if (ends_excmd(*skipwhite(p)))
		{
		    // "&option <NL>", "$ENV <NL>" and "@r <NL>" are the start
		    // of an expression.
		    eap->cmdidx = CMD_eval;
		    return eap->cmd;
		}
		// "&option" can be followed by "->" or "=", check below
	    }

	    swp = skipwhite(p);

	    if (
		// "(..." is an expression.
		// "funcname(" is always a function call.
		*p == '('
		    || (p == eap->cmd
			? (
			    // "{..." is a dict expression or block start.
			    *eap->cmd == '{'
			    // "'string'->func()" is an expression.
			 || *eap->cmd == '\''
			    // '"string"->func()' is an expression.
			 || (eap->cmd[0] == '0' && eap->cmd[1] == 'z')
			    // '"string"->func()' is an expression.
			 || *eap->cmd == '"'
			    // "g:varname" is an expression.
			 || eap->cmd[1] == ':'
			    )
			    // "varname->func()" is an expression.
			: (*swp == '-' && swp[1] == '>')))
	    {
		if (*eap->cmd == '{' && ends_excmd(*skipwhite(eap->cmd + 1)))
		{
		    // "{" by itself is the start of a block.
		    eap->cmdidx = CMD_block;
		    return eap->cmd + 1;
		}
		eap->cmdidx = CMD_eval;
		return eap->cmd;
	    }

	    if ((p != eap->cmd && (
			    // "varname[]" is an expression.
			    *p == '['
			    // "varname.key" is an expression.
			 || (*p == '.'
				     && (ASCII_ISALPHA(p[1]) || p[1] == '_'))))
			// g:[key] is an expression
		    || STRNCMP(eap->cmd, "g:[", 3) == 0)
	    {
		char_u	*after = eap->cmd;

		// When followed by "=" or "+=" then it is an assignment.
		// Skip over the whole thing, it can be:
		//	name.member = val
		//	name[a : b] = val
		//	name[idx] = val
		//	name[idx].member = val
		//	etc.
		eap->cmdidx = CMD_eval;
		++emsg_silent;
		if (skip_expr(&after, NULL) == OK)
		{
		    after = skipwhite(after);
		    if (*after == '=' || (*after != NUL && after[1] == '=')
					 || (after[0] == '.' && after[1] == '.'
							   && after[2] == '='))
			eap->cmdidx = CMD_var;
		}
		--emsg_silent;
		return eap->cmd;
	    }

	    // "[...]->Method()" is a list expression, but "[a, b] = Func()" is
	    // an assignment.
	    // If there is no line break inside the "[...]" then "p" is
	    // advanced to after the "]" by to_name_const_end(): check if a "="
	    // follows.
	    // If "[...]" has a line break "p" still points at the "[" and it
	    // can't be an assignment.
	    if (*eap->cmd == '[')
	    {
		char_u	    *eq;

		p = to_name_const_end(eap->cmd);
		if (p == eap->cmd && *p == '[')
		{
		    int count = 0;
		    int	semicolon = FALSE;

		    p = skip_var_list(eap->cmd, TRUE, &count, &semicolon, TRUE);
		}
		eq = p;
		if (eq != NULL)
		{
		    eq = skipwhite(eq);
		    if (vim_strchr((char_u *)"+-*/%", *eq) != NULL)
			++eq;
		}
		if (p == NULL || p == eap->cmd || *eq != '=')
		{
		    eap->cmdidx = CMD_eval;
		    return eap->cmd;
		}
		if (p > eap->cmd && *eq == '=')
		{
		    eap->cmdidx = CMD_var;
		    return eap->cmd;
		}
	    }

	    // Recognize an assignment if we recognize the variable name:
	    // "g:var = expr"
	    // "@r = expr"
	    // "&opt = expr"
	    // "var = expr"  where "var" is a variable name or we are skipping
	    // (variable declaration might have been skipped).
	    oplen = assignment_len(skipwhite(p), &heredoc);
	    if (oplen > 0)
	    {
		if (((p - eap->cmd) > 2 && eap->cmd[1] == ':')
			|| *eap->cmd == '&'
			|| *eap->cmd == '$'
			|| *eap->cmd == '@'
			|| eap->skip
			|| lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK)
		{
		    eap->cmdidx = CMD_var;
		    return eap->cmd;
		}
	    }

	    // Recognize using a type for a w:, b:, t: or g: variable:
	    // "w:varname: number = 123".
	    if (eap->cmd[1] == ':' && *p == ':')
	    {
		eap->cmdidx = CMD_eval;
		return eap->cmd;
	    }
	}

	// "g:", "s:" and "l:" are always assumed to be a variable, thus start
	// an expression.  A global/substitute/list command needs to use a
	// longer name.
	if (vim_strchr((char_u *)"gsl", *p) != NULL && p[1] == ':')
	{
	    eap->cmdidx = CMD_eval;
	    return eap->cmd;
	}

	// If it is an ID it might be a variable with an operator on the next
	// line, if the variable exists it can't be an Ex command.
	if (p > eap->cmd && ends_excmd(*skipwhite(p))
		&& (lookup(eap->cmd, p - eap->cmd, TRUE, cctx) == OK
		    || (ASCII_ISALPHA(eap->cmd[0]) && eap->cmd[1] == ':')))
	{
	    eap->cmdidx = CMD_eval;
	    return eap->cmd;
	}

	// Check for "++nr" and "--nr".
	if (p == eap->cmd && p[0] != NUL && p[0] == p[1]
						   && (*p == '+' || *p == '-'))
	{
	    eap->cmdidx = *p == '+' ? CMD_increment : CMD_decrement;
	    return eap->cmd + 2;
	}
    }
#endif

    /*
     * Isolate the command and search for it in the command table.
     */
    p = eap->cmd;
    if (one_letter_cmd(p, &eap->cmdidx))
    {
	++p;
    }
    else
    {
	while (ASCII_ISALPHA(*p))
	    ++p;
	// for python 3.x support ":py3", ":python3", ":py3file", etc.
	if (eap->cmd[0] == 'p' && eap->cmd[1] == 'y')
	{
	    while (ASCII_ISALNUM(*p))
		++p;
	}
	else if (*p == '9' && STRNCMP("vim9", eap->cmd, 4) == 0)
	{
	    // include "9" for "vim9*" commands; "vim9cmd" and "vim9script".
	    ++p;
	    while (ASCII_ISALPHA(*p))
		++p;
	}

	// check for non-alpha command
	if (p == eap->cmd && vim_strchr((char_u *)"@*!=><&~#}", *p) != NULL)
	    ++p;
	len = (int)(p - eap->cmd);
	// The "d" command can directly be followed by 'l' or 'p' flag, when
	// not in Vim9 script.
	if (!vim9 && *eap->cmd == 'd' && (p[-1] == 'l' || p[-1] == 'p'))
	{
	    // Check for ":dl", ":dell", etc. to ":deletel": that's
	    // :delete with the 'l' flag.  Same for 'p'.
	    for (i = 0; i < len; ++i)
		if (eap->cmd[i] != ((char_u *)"delete")[i])
		    break;
	    if (i == len - 1)
	    {
		--len;
		if (p[-1] == 'l')
		    eap->flags |= EXFLAG_LIST;
		else
		    eap->flags |= EXFLAG_PRINT;
	    }
	}

	if (ASCII_ISLOWER(eap->cmd[0]))
	{
	    int c1 = eap->cmd[0];
	    int c2 = len == 1 ? NUL : eap->cmd[1];

	    if (command_count != (int)CMD_SIZE)
	    {
		iemsg(_(e_command_table_needs_to_be_updated_run_make_cmdidxs));
		getout(1);
	    }

	    // Use a precomputed index for fast look-up in cmdnames[]
	    // taking into account the first 2 letters of eap->cmd.
	    eap->cmdidx = cmdidxs1[CharOrdLow(c1)];
	    if (ASCII_ISLOWER(c2))
		eap->cmdidx += cmdidxs2[CharOrdLow(c1)][CharOrdLow(c2)];
	}
	else if (ASCII_ISUPPER(eap->cmd[0]))
	    eap->cmdidx = CMD_Next;
	else
	    eap->cmdidx = CMD_bang;

	for ( ; (int)eap->cmdidx < (int)CMD_SIZE;
			       eap->cmdidx = (cmdidx_T)((int)eap->cmdidx + 1))
	    if (STRNCMP(cmdnames[(int)eap->cmdidx].cmd_name, (char *)eap->cmd,
							    (size_t)len) == 0)
	    {
#ifdef FEAT_EVAL
		if (full != NULL && cmdnames[eap->cmdidx].cmd_name[len] == NUL)
		    *full = TRUE;
#endif
		break;
	    }

	// :Print and :mode are not supported in Vim9 script.
	// Some commands cannot be shortened in Vim9 script.
	if (vim9 && eap->cmdidx != CMD_SIZE)
	{
	    if (eap->cmdidx == CMD_mode || eap->cmdidx == CMD_Print)
		eap->cmdidx = CMD_SIZE;
	    else if ((cmdnames[eap->cmdidx].cmd_argt & EX_WHOLE)
			  && len < (int)STRLEN(cmdnames[eap->cmdidx].cmd_name))
	    {
		semsg(_(e_command_cannot_be_shortened_str), eap->cmd);
		eap->cmdidx = CMD_SIZE;
	    }
	}

	// Do not recognize ":*" as the star command unless '*' is in
	// 'cpoptions'.
	if (eap->cmdidx == CMD_star && vim_strchr(p_cpo, CPO_STAR) == NULL)
	    p = eap->cmd;

	// Look for a user defined command as a last resort.  Let ":Print" be
	// overruled by a user defined command.
	if ((eap->cmdidx == CMD_SIZE || eap->cmdidx == CMD_Print)
		&& *eap->cmd >= 'A' && *eap->cmd <= 'Z')
	{
	    // User defined commands may contain digits.
	    while (ASCII_ISALNUM(*p))
		++p;
	    p = find_ucmd(eap, p, full, NULL, NULL);
	}
	if (p == NULL || p == eap->cmd)
	    eap->cmdidx = CMD_SIZE;
    }

    // ":fina" means ":finally" in legacy script, for backwards compatibility.
    if (eap->cmdidx == CMD_final && p - eap->cmd == 4 && !vim9)
	eap->cmdidx = CMD_finally;

#ifdef FEAT_EVAL
    if (eap->cmdidx < CMD_SIZE
	    && vim9
	    && !IS_WHITE_OR_NUL(*p) && *p != '\n' && *p != '!' && *p != '|'
	    && (eap->cmdidx < 0 ||
		(cmdnames[eap->cmdidx].cmd_argt & EX_NONWHITE_OK) == 0))
    {
	char_u *cmd = vim_strnsave(eap->cmd, p - eap->cmd);

	semsg(_(e_command_str_not_followed_by_white_space_str), cmd, eap->cmd);
	eap->cmdidx = CMD_SIZE;
	vim_free(cmd);
    }
#endif

    return p;
}