nv_ctrlg(cmdarg_T *cap)
{
    if (VIsual_active)	// toggle Selection/Visual mode
    {
	VIsual_select = !VIsual_select;
	showmode();
    }
    else if (!checkclearop(cap->oap))
	// print full name if count given or :cd used
	fileinfo((int)cap->count0, FALSE, TRUE);
}