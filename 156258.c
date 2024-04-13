sshpam_init_ctx(Authctxt *authctxt)
{
	struct pam_ctxt *ctxt;
	int socks[2];

	/* Initialize PAM */
	if (sshpam_init(authctxt->user) == -1) {
		error("PAM: initialization failed");
		return (NULL);
	}

	ctxt = xmalloc(sizeof *ctxt);
	ctxt->pam_done = 0;

	/* Start the authentication thread */
	if (socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, socks) == -1) {
		error("PAM: failed create sockets: %s", strerror(errno));
		xfree(ctxt);
		return (NULL);
	}
	ctxt->pam_psock = socks[0];
	ctxt->pam_csock = socks[1];
	if (pthread_create(&ctxt->pam_thread, NULL, sshpam_thread, ctxt) == -1) {
		error("PAM: failed to start authentication thread: %s",
		    strerror(errno));
		close(socks[0]);
		close(socks[1]);
		xfree(ctxt);
		return (NULL);
	}
	fatal_add_cleanup(sshpam_thread_cleanup, ctxt);
	return (ctxt);
}