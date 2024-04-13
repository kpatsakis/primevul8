normal_search(
    cmdarg_T	*cap,
    int		dir,
    char_u	*pat,
    int		opt,		// extra flags for do_search()
    int		*wrapped)
{
    int		i;
    searchit_arg_T sia;

    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    cap->oap->use_reg_one = TRUE;
    curwin->w_set_curswant = TRUE;

    CLEAR_FIELD(sia);
    i = do_search(cap->oap, dir, dir, pat, cap->count1,
			    opt | SEARCH_OPT | SEARCH_ECHO | SEARCH_MSG, &sia);
    if (wrapped != NULL)
	*wrapped = sia.sa_wrapped;
    if (i == 0)
	clearop(cap->oap);
    else
    {
	if (i == 2)
	    cap->oap->motion_type = MLINE;
	curwin->w_cursor.coladd = 0;
#ifdef FEAT_FOLDING
	if (cap->oap->op_type == OP_NOP && (fdo_flags & FDO_SEARCH) && KeyTyped)
	    foldOpenCursor();
#endif
    }

    // "/$" will put the cursor after the end of the line, may need to
    // correct that here
    check_cursor();
    return i;
}