gpg_ctx_op_cancel (struct _GpgCtx *gpg)
{
#ifndef G_OS_WIN32
	pid_t retval;
	gint status;

	if (gpg->exited)
		return;

	kill (gpg->pid, SIGTERM);
	sleep (1);
	retval = waitpid (gpg->pid, &status, WNOHANG);

	if (retval == (pid_t) 0) {
		/* no more mr nice guy... */
		kill (gpg->pid, SIGKILL);
		sleep (1);
		waitpid (gpg->pid, &status, WNOHANG);
	}
#endif
}