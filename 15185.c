eval_foldexpr(win_T *wp, int *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    int		use_sandbox = was_set_insecurely((char_u *)"foldexpr",
								   OPT_LOCAL);

    arg = wp->w_p_fde;
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    if (use_sandbox)
	++sandbox;
    ++textwinlock;
    *cp = NUL;
    if (eval0(arg, &tv, NULL, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    else
    {
	// If the result is a number, just return the number.
	if (tv.v_type == VAR_NUMBER)
	    retval = tv.vval.v_number;
	else if (tv.v_type != VAR_STRING || tv.vval.v_string == NULL)
	    retval = 0;
	else
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    if (!VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((char *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    if (use_sandbox)
	--sandbox;
    --textwinlock;
    clear_evalarg(&EVALARG_EVALUATE, NULL);
    current_sctx = saved_sctx;

    return (int)retval;
}