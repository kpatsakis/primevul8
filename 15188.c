eval_expr_typval(typval_T *expr, typval_T *argv, int argc, typval_T *rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];
    funcexe_T	funcexe;

    if (expr->v_type == VAR_FUNC)
    {
	s = expr->vval.v_string;
	if (s == NULL || *s == NUL)
	    return FAIL;
	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	if (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    return FAIL;
    }
    else if (expr->v_type == VAR_PARTIAL)
    {
	partial_T   *partial = expr->vval.v_partial;

	if (partial == NULL)
	    return FAIL;

	if (partial->pt_func != NULL
			  && partial->pt_func->uf_def_status != UF_NOT_COMPILED)
	{
	    if (call_def_function(partial->pt_func, argc, argv,
						       partial, rettv) == FAIL)
		return FAIL;
	}
	else
	{
	    s = partial_name(partial);
	    if (s == NULL || *s == NUL)
		return FAIL;
	    CLEAR_FIELD(funcexe);
	    funcexe.fe_evaluate = TRUE;
	    funcexe.fe_partial = partial;
	    if (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
		return FAIL;
	}
    }
    else if (expr->v_type == VAR_INSTR)
    {
	return exe_typval_instr(expr, rettv);
    }
    else
    {
	s = tv_get_string_buf_chk(expr, buf);
	if (s == NULL)
	    return FAIL;
	s = skipwhite(s);
	if (eval1_emsg(&s, rettv, NULL) == FAIL)
	    return FAIL;
	if (*skipwhite(s) != NUL)  // check for trailing chars after expr
	{
	    clear_tv(rettv);
	    semsg(_(e_invalid_expression_str), s);
	    return FAIL;
	}
    }
    return OK;
}