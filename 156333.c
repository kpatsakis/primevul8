abandon_challenge_response(Authctxt *authctxt)
{
	if (authctxt->kbdintctxt != NULL) {
		device->free_ctx(authctxt->kbdintctxt);
		authctxt->kbdintctxt = NULL;
	}
}