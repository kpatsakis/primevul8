nv_colon(cmdarg_T *cap)
{
    int	old_p_im;
    int	cmd_result;
    int	is_cmdkey = cap->cmdchar == K_COMMAND;

    if (VIsual_active && !is_cmdkey)
	nv_operator(cap);
    else
    {
	if (cap->oap->op_type != OP_NOP)
	{
	    // Using ":" as a movement is characterwise exclusive.
	    cap->oap->motion_type = MCHAR;
	    cap->oap->inclusive = FALSE;
	}
	else if (cap->count0 && !is_cmdkey)
	{
	    // translate "count:" into ":.,.+(count - 1)"
	    stuffcharReadbuff('.');
	    if (cap->count0 > 1)
	    {
		stuffReadbuff((char_u *)",.+");
		stuffnumReadbuff((long)cap->count0 - 1L);
	    }
	}

	// When typing, don't type below an old message
	if (KeyTyped)
	    compute_cmdrow();

	old_p_im = p_im;

	// get a command line and execute it
	cmd_result = do_cmdline(NULL, is_cmdkey ? getcmdkeycmd : getexline, NULL,
			    cap->oap->op_type != OP_NOP ? DOCMD_KEEPLINE : 0);

	// If 'insertmode' changed, enter or exit Insert mode
	if (p_im != old_p_im)
	{
	    if (p_im)
		restart_edit = 'i';
	    else
		restart_edit = 0;
	}

	if (cmd_result == FAIL)
	    // The Ex command failed, do not execute the operator.
	    clearop(cap->oap);
	else if (cap->oap->op_type != OP_NOP
		&& (cap->oap->start.lnum > curbuf->b_ml.ml_line_count
		    || cap->oap->start.col >
			       (colnr_T)STRLEN(ml_get(cap->oap->start.lnum))
		    || did_emsg
		    ))
	    // The start of the operator has become invalid by the Ex command.
	    clearopbeep(cap->oap);
    }
}