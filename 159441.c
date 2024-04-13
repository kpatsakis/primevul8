nv_select(cmdarg_T *cap)
{
    if (VIsual_active)
	VIsual_select = TRUE;
    else if (VIsual_reselect)
    {
	cap->nchar = 'v';	    // fake "gv" command
	cap->arg = TRUE;
	nv_g_cmd(cap);
    }
}