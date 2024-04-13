alloc_tabpage(void)
{
    tabpage_T	*tp;
# ifdef FEAT_GUI
    int		i;
# endif


    tp = ALLOC_CLEAR_ONE(tabpage_T);
    if (tp == NULL)
	return NULL;

# ifdef FEAT_EVAL
    /* init t: variables */
    tp->tp_vars = dict_alloc();
    if (tp->tp_vars == NULL)
    {
	vim_free(tp);
	return NULL;
    }
    init_var_dict(tp->tp_vars, &tp->tp_winvar, VAR_SCOPE);
# endif

# ifdef FEAT_GUI
    for (i = 0; i < 3; i++)
	tp->tp_prev_which_scrollbars[i] = -1;
# endif
# ifdef FEAT_DIFF
    tp->tp_diff_invalid = TRUE;
# endif
    tp->tp_ch_used = p_ch;

    return tp;
}