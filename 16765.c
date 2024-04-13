apply_cmdmod(cmdmod_T *cmod)
{
#ifdef HAVE_SANDBOX
    if ((cmod->cmod_flags & CMOD_SANDBOX) && !cmod->cmod_did_sandbox)
    {
	++sandbox;
	cmod->cmod_did_sandbox = TRUE;
    }
#endif
    if (cmod->cmod_verbose > 0)
    {
	if (cmod->cmod_verbose_save == 0)
	    cmod->cmod_verbose_save = p_verbose + 1;
	p_verbose = cmod->cmod_verbose - 1;
    }

    if ((cmod->cmod_flags & (CMOD_SILENT | CMOD_UNSILENT))
	    && cmod->cmod_save_msg_silent == 0)
    {
	cmod->cmod_save_msg_silent = msg_silent + 1;
	cmod->cmod_save_msg_scroll = msg_scroll;
    }
    if (cmod->cmod_flags & CMOD_SILENT)
	++msg_silent;
    if (cmod->cmod_flags & CMOD_UNSILENT)
	msg_silent = 0;

    if (cmod->cmod_flags & CMOD_ERRSILENT)
    {
	++emsg_silent;
	++cmod->cmod_did_esilent;
    }

    if ((cmod->cmod_flags & CMOD_NOAUTOCMD) && cmod->cmod_save_ei == NULL)
    {
	// Set 'eventignore' to "all".
	// First save the existing option value for restoring it later.
	cmod->cmod_save_ei = vim_strsave(p_ei);
	set_string_option_direct((char_u *)"ei", -1,
					  (char_u *)"all", OPT_FREE, SID_NONE);
    }
}