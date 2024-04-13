find_script_callback(char_u *fname, void *cookie)
{
    int sid;
    int error = OK;
    int *ret_sid = cookie;

    sid = find_script_by_name(fname);
    if (sid < 0)
    {
	// script does not exist yet, create a new scriptitem
	sid = get_new_scriptitem(&error);
	if (error == OK)
	{
	    scriptitem_T *si = SCRIPT_ITEM(sid);

	    si->sn_name = vim_strsave(fname);
	    si->sn_state = SN_STATE_NOT_LOADED;
	}
    }
    *ret_sid = sid;
}