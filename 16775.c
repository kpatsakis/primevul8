ex_startinsert(exarg_T *eap)
{
    if (eap->forceit)
    {
	// cursor line can be zero on startup
	if (!curwin->w_cursor.lnum)
	    curwin->w_cursor.lnum = 1;
	set_cursor_for_append_to_line();
    }
#ifdef FEAT_TERMINAL
    // Ignore this when running in an active terminal.
    if (term_job_running(curbuf->b_term))
	return;
#endif

    // Ignore the command when already in Insert mode.  Inserting an
    // expression register that invokes a function can do this.
    if (State & MODE_INSERT)
	return;

    if (eap->cmdidx == CMD_startinsert)
	restart_edit = 'a';
    else if (eap->cmdidx == CMD_startreplace)
	restart_edit = 'R';
    else
	restart_edit = 'V';

    if (!eap->forceit)
    {
	if (eap->cmdidx == CMD_startinsert)
	    restart_edit = 'i';
	curwin->w_curswant = 0;	    // avoid MAXCOL
    }

    if (VIsual_active)
	showmode();
}