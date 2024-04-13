skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    int	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    if (evalarg == NULL)
	return skipwhite(arg);
    eval_next_non_blank(p, evalarg, &getnext);
    if (getnext)
	return eval_next_line(evalarg);
    return p;
}