tgetpass_display_error(enum tgetpass_errval errval)
{
    debug_decl(tgetpass_display_error, SUDO_DEBUG_CONV);

    switch (errval) {
    case TGP_ERRVAL_NOERROR:
	break;
    case TGP_ERRVAL_TIMEOUT:
	sudo_warnx(U_("timed out reading password"));
	break;
    case TGP_ERRVAL_NOPASSWORD:
	sudo_warnx(U_("no password was provided"));
	break;
    case TGP_ERRVAL_READERROR:
	sudo_warn(U_("unable to read password"));
	break;
    }
    debug_return;
}