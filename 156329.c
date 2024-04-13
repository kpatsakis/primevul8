send_userauth_info_request(Authctxt *authctxt)
{
	KbdintAuthctxt *kbdintctxt;
	char *name, *instr, **prompts;
	int i;
	u_int *echo_on;

	kbdintctxt = authctxt->kbdintctxt;
	if (kbdintctxt->device->query(kbdintctxt->ctxt,
	    &name, &instr, &kbdintctxt->nreq, &prompts, &echo_on))
		return 0;

	packet_start(SSH2_MSG_USERAUTH_INFO_REQUEST);
	packet_put_cstring(name);
	packet_put_cstring(instr);
	packet_put_cstring("");		/* language not used */
	packet_put_int(kbdintctxt->nreq);
	for (i = 0; i < kbdintctxt->nreq; i++) {
		packet_put_cstring(prompts[i]);
		packet_put_char(echo_on[i]);
	}
	packet_send();
	packet_write_wait();

	for (i = 0; i < kbdintctxt->nreq; i++)
		xfree(prompts[i]);
	xfree(prompts);
	xfree(echo_on);
	xfree(name);
	xfree(instr);
	return 1;
}