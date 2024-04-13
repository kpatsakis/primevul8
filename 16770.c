changedir_func(
	char_u		*new_dir,
	int		forceit,
	cdscope_T	scope)
{
    char_u	*pdir = NULL;
    int		dir_differs;
    char_u	*acmd_fname = NULL;
    char_u	**pp;
    char_u	*tofree;

    if (new_dir == NULL || allbuf_locked())
	return FALSE;

    if (vim_strchr(p_cpo, CPO_CHDIR) != NULL && curbufIsChanged() && !forceit)
    {
	emsg(_(e_cannot_change_directory_buffer_is_modified_add_bang_to_override));
	return FALSE;
    }

    // ":cd -": Change to previous directory
    if (STRCMP(new_dir, "-") == 0)
    {
	pdir = get_prevdir(scope);
	if (pdir == NULL)
	{
	    emsg(_(e_no_previous_directory));
	    return FALSE;
	}
	new_dir = pdir;
    }

    // Save current directory for next ":cd -"
    if (mch_dirname(NameBuff, MAXPATHL) == OK)
	pdir = vim_strsave(NameBuff);
    else
	pdir = NULL;

    // For UNIX ":cd" means: go to home directory.
    // On other systems too if 'cdhome' is set.
#if defined(UNIX) || defined(VMS)
    if (*new_dir == NUL)
#else
    if (*new_dir == NUL && p_cdh)
#endif
    {
	// use NameBuff for home directory name
# ifdef VMS
	char_u	*p;

	p = mch_getenv((char_u *)"SYS$LOGIN");
	if (p == NULL || *p == NUL)	// empty is the same as not set
	    NameBuff[0] = NUL;
	else
	    vim_strncpy(NameBuff, p, MAXPATHL - 1);
# else
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
# endif
	new_dir = NameBuff;
    }
    dir_differs = pdir == NULL
			    || pathcmp((char *)pdir, (char *)new_dir, -1) != 0;
    if (dir_differs)
    {
	if (scope == CDSCOPE_WINDOW)
	    acmd_fname = (char_u *)"window";
	else if (scope == CDSCOPE_TABPAGE)
	    acmd_fname = (char_u *)"tabpage";
	else
	    acmd_fname = (char_u *)"global";
	trigger_DirChangedPre(acmd_fname, new_dir);

	if (vim_chdir(new_dir))
	{
	    emsg(_(e_command_failed));
	    vim_free(pdir);
	    return FALSE;
	}
    }

    if (scope == CDSCOPE_WINDOW)
	pp = &curwin->w_prevdir;
    else if (scope == CDSCOPE_TABPAGE)
	pp = &curtab->tp_prevdir;
    else
	pp = &prev_dir;
    tofree = *pp;  // new_dir may use this
    *pp = pdir;

    post_chdir(scope);

    if (dir_differs)
	apply_autocmds(EVENT_DIRCHANGED, acmd_fname, new_dir, FALSE, curbuf);
    vim_free(tofree);
    return TRUE;
}