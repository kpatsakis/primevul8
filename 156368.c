sshpam_free_ctx(void *ctxtp)
{
	struct pam_ctxt *ctxt = ctxtp;

	fatal_remove_cleanup(sshpam_thread_cleanup, ctxt);
	sshpam_thread_cleanup(ctxtp);
	xfree(ctxt);
	/*
	 * We don't call sshpam_cleanup() here because we may need the PAM
	 * handle at a later stage, e.g. when setting up a session.  It's
	 * still on the cleanup list, so pam_end() *will* be called before
	 * the server process terminates.
	 */
}