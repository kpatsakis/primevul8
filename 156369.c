auth2_challenge(Authctxt *authctxt, char *devs)
{
	debug("auth2_challenge: user=%s devs=%s",
	    authctxt->user ? authctxt->user : "<nouser>",
	    devs ? devs : "<no devs>");

	if (authctxt->user == NULL || !devs)
		return 0;
	if (authctxt->kbdintctxt == NULL)
		authctxt->kbdintctxt = kbdint_alloc(devs);
	return auth2_challenge_start(authctxt);
}