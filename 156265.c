sshpam_thread_cleanup(void *ctxtp)
{
	struct pam_ctxt *ctxt = ctxtp;

	pthread_cancel(ctxt->pam_thread);
	pthread_join(ctxt->pam_thread, NULL);
	close(ctxt->pam_psock);
	close(ctxt->pam_csock);
}