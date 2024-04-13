nv_exmode(cmdarg_T *cap)
{
    /*
     * Ignore 'Q' in Visual mode, just give a beep.
     */
    if (VIsual_active)
	vim_beep(BO_EX);
    else if (!checkclearop(cap->oap))
	do_exmode(FALSE);
}