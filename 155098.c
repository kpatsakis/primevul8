free_tabpage(tabpage_T *tp)
{
    int idx;

# ifdef FEAT_DIFF
    diff_clear(tp);
# endif
# ifdef FEAT_TEXT_PROP
    while (tp->tp_first_popupwin != NULL)
	popup_close_tabpage(tp, tp->tp_first_popupwin->w_id);
#endif
    for (idx = 0; idx < SNAP_COUNT; ++idx)
	clear_snapshot(tp, idx);
#ifdef FEAT_EVAL
    vars_clear(&tp->tp_vars->dv_hashtab);	/* free all t: variables */
    hash_init(&tp->tp_vars->dv_hashtab);
    unref_var_dict(tp->tp_vars);
#endif

    vim_free(tp->tp_localdir);

#ifdef FEAT_PYTHON
    python_tabpage_free(tp);
#endif

#ifdef FEAT_PYTHON3
    python3_tabpage_free(tp);
#endif

    vim_free(tp);
}