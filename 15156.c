eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    int	    getnext;

    CLEAR_POINTER(rettv);

    /*
     * Get the first variable.
     */
    if (eval2(arg, rettv, evalarg) == FAIL)
	return FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);
    if (*p == '?')
    {
	int		op_falsy = p[1] == '?';
	int		result;
	typval_T	var2;
	evalarg_T	*evalarg_used = evalarg;
	evalarg_T	local_evalarg;
	int		orig_flags;
	int		evaluate;
	int		vim9script = in_vim9script();

	if (evalarg == NULL)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = evalarg_used->eval_flags & EVAL_EVALUATE;

	if (getnext)
	    *arg = eval_next_line(evalarg_used);
	else
	{
	    if (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		return FAIL;
	    }
	    *arg = p;
	}

	result = FALSE;
	if (evaluate)
	{
	    int		error = FALSE;

	    if (op_falsy)
		result = tv2bool(rettv);
	    else if (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    else if (tv_get_number_chk(rettv, &error) != 0)
		result = TRUE;
	    if (error || !op_falsy || !result)
		clear_tv(rettv);
	    if (error)
		return FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	if (op_falsy)
	    ++*arg;
	if (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    return FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	evalarg_used->eval_flags = (op_falsy ? !result : result)
				    ? orig_flags : orig_flags & ~EVAL_EVALUATE;
	if (eval1(arg, &var2, evalarg_used) == FAIL)
	{
	    evalarg_used->eval_flags = orig_flags;
	    return FAIL;
	}
	if (!op_falsy || !result)
	    *rettv = var2;

	if (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	    if (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		if (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		return FAIL;
	    }
	    if (getnext)
		*arg = eval_next_line(evalarg_used);
	    else
	    {
		if (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_used->eval_flags = orig_flags;
		    return FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    if (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		return FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
						 : orig_flags & ~EVAL_EVALUATE;
	    if (eval1(arg, &var2, evalarg_used) == FAIL)
	    {
		if (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		return FAIL;
	    }
	    if (evaluate && !result)
		*rettv = var2;
	}

	if (evalarg == NULL)
	    clear_evalarg(&local_evalarg, NULL);
	else
	    evalarg->eval_flags = orig_flags;
    }

    return OK;
}