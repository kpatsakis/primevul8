call_vim_function(
    char_u      *func,
    int		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    int		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = NULL;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".
    arg = func;
    ++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    --emsg_off;
    if (name == NULL)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    if (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    return ret;
}