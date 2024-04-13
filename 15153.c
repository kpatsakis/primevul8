eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first variable.
     */
    if (eval6(arg, rettv, evalarg, FALSE) == FAIL)
	return FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    for (;;)
    {
	int	    evaluate;
	int	    getnext;
	char_u	    *p;
	int	    op;
	int	    oplen;
	int	    concat;
	typval_T    var2;
	int	    vim9script = in_vim9script();

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || in_old_script(2));
	if ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    break;
	if (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    break;

	evaluate = evalarg == NULL ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	if (getnext)
	    *arg = eval_next_line(evalarg);
	else
	{
	    if (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		return FAIL;
	    }
	    *arg = p;
	}
	if ((op != '+' || (rettv->v_type != VAR_LIST
						 && rettv->v_type != VAR_BLOB))
#ifdef FEAT_FLOAT
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
#endif
		&& evaluate)
	{
	    int		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    if (op != '.')
		tv_get_number_chk(rettv, &error);
	    if ((op == '.' && tv_get_string_chk(rettv) == NULL) || error)
	    {
		clear_tv(rettv);
		return FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	if (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    return FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	if (eval6(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    return FAIL;
	}

	if (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    if (op == '.')
	    {
		char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
		char_u	*s1 = tv_get_string_buf(rettv, buf1);
		char_u	*s2 = NULL;

		if (vim9script && (var2.v_type == VAR_VOID
			|| var2.v_type == VAR_CHANNEL
			|| var2.v_type == VAR_JOB))
		    semsg(_(e_using_invalid_value_as_string_str),
						   vartype_name(var2.v_type));
#ifdef FEAT_FLOAT
		else if (vim9script && var2.v_type == VAR_FLOAT)
		{
		    vim_snprintf((char *)buf2, NUMBUFLEN, "%g",
							    var2.vval.v_float);
		    s2 = buf2;
		}
#endif
		else
		    s2 = tv_get_string_buf_chk(&var2, buf2);
		if (s2 == NULL)		// type error ?
		{
		    clear_tv(rettv);
		    clear_tv(&var2);
		    return FAIL;
		}
		p = concat_str(s1, s2);
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = p;
	    }
	    else if (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    else if (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		if (eval_addlist(rettv, &var2) == FAIL)
		    return FAIL;
	    }
	    else
	    {
		int		error = FALSE;
		varnumber_T	n1, n2;
#ifdef FEAT_FLOAT
		float_T	    f1 = 0, f2 = 0;

		if (rettv->v_type == VAR_FLOAT)
		{
		    f1 = rettv->vval.v_float;
		    n1 = 0;
		}
		else
#endif
		{
		    n1 = tv_get_number_chk(rettv, &error);
		    if (error)
		    {
			// This can only happen for "list + non-list" or
			// "blob + non-blob".  For "non-list + ..." or
			// "something - ...", we returned before evaluating the
			// 2nd operand.
			clear_tv(rettv);
			clear_tv(&var2);
			return FAIL;
		    }
#ifdef FEAT_FLOAT
		    if (var2.v_type == VAR_FLOAT)
			f1 = n1;
#endif
		}
#ifdef FEAT_FLOAT
		if (var2.v_type == VAR_FLOAT)
		{
		    f2 = var2.vval.v_float;
		    n2 = 0;
		}
		else
#endif
		{
		    n2 = tv_get_number_chk(&var2, &error);
		    if (error)
		    {
			clear_tv(rettv);
			clear_tv(&var2);
			return FAIL;
		    }
#ifdef FEAT_FLOAT
		    if (rettv->v_type == VAR_FLOAT)
			f2 = n2;
#endif
		}
		clear_tv(rettv);

#ifdef FEAT_FLOAT
		// If there is a float on either side the result is a float.
		if (rettv->v_type == VAR_FLOAT || var2.v_type == VAR_FLOAT)
		{
		    if (op == '+')
			f1 = f1 + f2;
		    else
			f1 = f1 - f2;
		    rettv->v_type = VAR_FLOAT;
		    rettv->vval.v_float = f1;
		}
		else
#endif
		{
		    if (op == '+')
			n1 = n1 + n2;
		    else
			n1 = n1 - n2;
		    rettv->v_type = VAR_NUMBER;
		    rettv->vval.v_number = n1;
		}
	    }
	    clear_tv(&var2);
	}
    }
    return OK;
}