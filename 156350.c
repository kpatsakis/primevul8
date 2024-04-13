auth2_challenge_start(Authctxt *authctxt)
{
	KbdintAuthctxt *kbdintctxt = authctxt->kbdintctxt;

	debug2("auth2_challenge_start: devices %s",
	    kbdintctxt->devices ?  kbdintctxt->devices : "<empty>");

	if (kbdint_next_device(kbdintctxt) == 0) {
		auth2_challenge_stop(authctxt);
		return 0;
	}
	debug("auth2_challenge_start: trying authentication method '%s'",
	    kbdintctxt->device->name);

	if ((kbdintctxt->ctxt = kbdintctxt->device->init_ctx(authctxt)) == NULL) {
		auth2_challenge_stop(authctxt);
		return 0;
	}
	if (send_userauth_info_request(authctxt) == 0) {
		auth2_challenge_stop(authctxt);
		return 0;
	}
	dispatch_set(SSH2_MSG_USERAUTH_INFO_RESPONSE,
	    &input_userauth_info_response);

	authctxt->postponed = 1;
	return 0;
}