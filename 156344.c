verify_response(Authctxt *authctxt, const char *response)
{
	char *resp[1];
	int res;

	if (device == NULL)
		return 0;
	if (authctxt->kbdintctxt == NULL)
		return 0;
	resp[0] = (char *)response;
	res = device->respond(authctxt->kbdintctxt, 1, resp);
	if (res == 1) {
		/* postponed - send a null query just in case */
		char *name, *info, **prompts;
		u_int i, numprompts, *echo_on;

		res = device->query(authctxt->kbdintctxt, &name, &info,
		    &numprompts, &prompts, &echo_on);
		if (res == 0) {
			for (i = 0; i < numprompts; i++)
				xfree(prompts[i]);
			xfree(prompts);
			xfree(name);
			xfree(echo_on);
			xfree(info);
		}
		/* if we received more prompts, we're screwed */
		res = (numprompts != 0);
	}
	device->free_ctx(authctxt->kbdintctxt);
	authctxt->kbdintctxt = NULL;
	return res ? 0 : 1;
}