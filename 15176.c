eval_next_non_blank(char_u *arg, evalarg_T *evalarg, int *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    if (in_vim9script()
	    && evalarg != NULL
	    && (evalarg->eval_cookie != NULL || evalarg->eval_cctx != NULL)
	    && (*p == NUL || (vim9_comment_start(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *next;

	if (evalarg->eval_cookie != NULL)
	    next = getline_peek_skip_comments(evalarg);
	else
	    next = peek_next_line_from_context(evalarg->eval_cctx);

	if (next != NULL)
	{
	    *getnext = TRUE;
	    return skipwhite(next);
	}
    }
    return p;
}