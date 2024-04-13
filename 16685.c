script_autoload(
    char_u	*name,
    int		reload)	    // load script again when already loaded
{
    char_u	*p;
    char_u	*scriptname, *tofree;
    int		ret = FALSE;
    int		i;
    int		ret_sid;

    // If there is no '#' after name[0] there is no package name.
    p = vim_strchr(name, AUTOLOAD_CHAR);
    if (p == NULL || p == name)
	return FALSE;

    tofree = scriptname = autoload_name(name);
    if (scriptname == NULL)
	return FALSE;

    // Find the name in the list of previously loaded package names.  Skip
    // "autoload/", it's always the same.
    for (i = 0; i < ga_loaded.ga_len; ++i)
	if (STRCMP(((char_u **)ga_loaded.ga_data)[i] + 9, scriptname + 9) == 0)
	    break;
    if (!reload && i < ga_loaded.ga_len)
	ret = FALSE;	    // was loaded already
    else
    {
	// Remember the name if it wasn't loaded already.
	if (i == ga_loaded.ga_len && ga_grow(&ga_loaded, 1) == OK)
	{
	    ((char_u **)ga_loaded.ga_data)[ga_loaded.ga_len++] = scriptname;
	    tofree = NULL;
	}

	// Try loading the package from $VIMRUNTIME/autoload/<name>.vim
	// Use "ret_sid" to avoid loading the same script again.
	if (source_in_path(p_rtp, scriptname, DIP_START, &ret_sid) == OK)
	    ret = TRUE;
    }

    vim_free(tofree);
    return ret;
}