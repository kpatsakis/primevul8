skip_for_lines(void *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    int		i;

    for (i = 0; i < fi->fi_break_count; ++i)
	eval_next_line(evalarg);
}