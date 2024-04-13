nv_ctrlo(cmdarg_T *cap)
{
    if (VIsual_active && VIsual_select)
    {
	VIsual_select = FALSE;
	showmode();
	restart_VIsual_select = 2;	// restart Select mode later
    }
    else
    {
	cap->count1 = -cap->count1;
	nv_pcmark(cap);
    }
}