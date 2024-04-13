auth2_challenge_stop(Authctxt *authctxt)
{
	/* unregister callback */
	dispatch_set(SSH2_MSG_USERAUTH_INFO_RESPONSE, NULL);
	if (authctxt->kbdintctxt != NULL)  {
		kbdint_free(authctxt->kbdintctxt);
		authctxt->kbdintctxt = NULL;
	}
}