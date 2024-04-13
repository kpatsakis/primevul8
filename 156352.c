userauth_kbdint(Authctxt *authctxt)
{
	int authenticated = 0;
	char *lang, *devs;

	lang = packet_get_string(NULL);
	devs = packet_get_string(NULL);
	packet_check_eom();

	debug("keyboard-interactive devs %s", devs);

	if (options.challenge_response_authentication)
		authenticated = auth2_challenge(authctxt, devs);

	xfree(devs);
	xfree(lang);
#ifdef HAVE_CYGWIN
	if (check_nt_auth(0, authctxt->pw) == 0)
		return(0);
#endif
	return authenticated;
}