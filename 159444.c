nv_dot(cmdarg_T *cap)
{
    if (!checkclearopq(cap->oap))
    {
	/*
	 * If "restart_edit" is TRUE, the last but one command is repeated
	 * instead of the last command (inserting text). This is used for
	 * CTRL-O <.> in insert mode.
	 */
	if (start_redo(cap->count0, restart_edit != 0 && !arrow_used) == FAIL)
	    clearopbeep(cap->oap);
    }
}