get_challenge(Authctxt *authctxt)
{
	char *challenge, *name, *info, **prompts;
	u_int i, numprompts;
	u_int *echo_on;

	device = devices[0]; /* we always use the 1st device for protocol 1 */
	if (device == NULL)
		return NULL;
	if ((authctxt->kbdintctxt = device->init_ctx(authctxt)) == NULL)
		return NULL;
	if (device->query(authctxt->kbdintctxt, &name, &info,
	    &numprompts, &prompts, &echo_on)) {
		device->free_ctx(authctxt->kbdintctxt);
		authctxt->kbdintctxt = NULL;
		return NULL;
	}
	if (numprompts < 1)
		fatal("get_challenge: numprompts < 1");
	challenge = xstrdup(prompts[0]);
	for (i = 0; i < numprompts; i++)
		xfree(prompts[i]);
	xfree(prompts);
	xfree(name);
	xfree(echo_on);
	xfree(info);

	return (challenge);
}