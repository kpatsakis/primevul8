gpg_ctx_op_exited (struct _GpgCtx *gpg)
{
	pid_t retval;
	gint status;

	if (gpg->exited)
		return TRUE;

	retval = waitpid (gpg->pid, &status, WNOHANG);
	if (retval == gpg->pid) {
		gpg->exit_status = status;
		gpg->exited = TRUE;
		return TRUE;
	}

	return FALSE;
}