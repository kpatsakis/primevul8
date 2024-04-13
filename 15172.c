fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, int skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;
    if (eap != NULL)
    {
	evalarg->eval_cstack = eap->cstack;
	if (getline_equal(eap->getline, eap->cookie, getsourceline))
	{
	    evalarg->eval_getline = eap->getline;
	    evalarg->eval_cookie = eap->cookie;
	}
    }
}