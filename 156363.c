input_userauth_info_response(int type, u_int32_t seq, void *ctxt)
{
	Authctxt *authctxt = ctxt;
	KbdintAuthctxt *kbdintctxt;
	int i, authenticated = 0, res, len;
	u_int nresp;
	char **response = NULL, *method;

	if (authctxt == NULL)
		fatal("input_userauth_info_response: no authctxt");
	kbdintctxt = authctxt->kbdintctxt;
	if (kbdintctxt == NULL || kbdintctxt->ctxt == NULL)
		fatal("input_userauth_info_response: no kbdintctxt");
	if (kbdintctxt->device == NULL)
		fatal("input_userauth_info_response: no device");

	authctxt->postponed = 0;	/* reset */
	nresp = packet_get_int();
	if (nresp != kbdintctxt->nreq)
		fatal("input_userauth_info_response: wrong number of replies");
	if (nresp > 100)
		fatal("input_userauth_info_response: too many replies");
	if (nresp > 0) {
		response = xmalloc(nresp * sizeof(char *));
		for (i = 0; i < nresp; i++)
			response[i] = packet_get_string(NULL);
	}
	packet_check_eom();

	if (authctxt->valid) {
		res = kbdintctxt->device->respond(kbdintctxt->ctxt,
		    nresp, response);
	} else {
		res = -1;
	}

	for (i = 0; i < nresp; i++) {
		memset(response[i], 'r', strlen(response[i]));
		xfree(response[i]);
	}
	if (response)
		xfree(response);

	switch (res) {
	case 0:
		/* Success! */
		authenticated = 1;
		break;
	case 1:
		/* Authentication needs further interaction */
		if (send_userauth_info_request(authctxt) == 1)
			authctxt->postponed = 1;
		break;
	default:
		/* Failure! */
		break;
	}

	len = strlen("keyboard-interactive") + 2 +
		strlen(kbdintctxt->device->name);
	method = xmalloc(len);
	snprintf(method, len, "keyboard-interactive/%s",
	    kbdintctxt->device->name);

	if (!authctxt->postponed) {
		if (authenticated) {
			auth2_challenge_stop(authctxt);
		} else {
			/* start next device */
			/* may set authctxt->postponed */
			auth2_challenge_start(authctxt);
		}
	}
	userauth_finish(authctxt, authenticated, method);
	xfree(method);
}