eval_method(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    int		verbose)	// give error messages
{
    char_u	*name;
    long	len;
    char_u	*alias;
    char_u	*tofree = NULL;
    typval_T	base = *rettv;
    int		ret = OK;
    int		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    if (alias != NULL)
	name = alias;

    if (len <= 0)
    {
	if (verbose)
	    emsg(_(e_missing_name_after_method));
	ret = FAIL;
    }
    else
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	if (**arg != '(' && alias == NULL
				    && (paren = vim_strchr(*arg, '(')) != NULL)
	{
	    char_u *deref;

	    *arg = name;
	    *paren = NUL;
	    deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    if (deref == NULL)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    else
	    {
		name = deref;
		len = STRLEN(name);
	    }
	    *paren = '(';
	}

	if (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    if (**arg != '(')
	    {
		if (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    else if (VIM_ISWHITE((*arg)[-1]))
	    {
		if (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    else
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &base);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    if (evaluate)
	clear_tv(&base);
    vim_free(tofree);

    return ret;
}