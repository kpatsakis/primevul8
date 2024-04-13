gpg_ctx_op_wait (struct _GpgCtx *gpg)
{
#ifndef G_OS_WIN32
	sigset_t mask, omask;
	pid_t retval;
	gint status;

	if (!gpg->exited) {
		sigemptyset (&mask);
		sigaddset (&mask, SIGALRM);
		sigprocmask (SIG_BLOCK, &mask, &omask);
		alarm (1);
		retval = waitpid (gpg->pid, &status, 0);
		alarm (0);
		sigprocmask (SIG_SETMASK, &omask, NULL);

		if (retval == (pid_t) -1 && errno == EINTR) {
			/* The child is hanging: send a friendly reminder. */
			kill (gpg->pid, SIGTERM);
			sleep (1);
			retval = waitpid (gpg->pid, &status, WNOHANG);
			if (retval == (pid_t) 0) {
				/* Still hanging; use brute force. */
				kill (gpg->pid, SIGKILL);
				sleep (1);
				retval = waitpid (gpg->pid, &status, WNOHANG);
			}
		}
	} else {
		status = gpg->exit_status;
		retval = gpg->pid;
	}

	if (retval != (pid_t) -1 && WIFEXITED (status))
		return WEXITSTATUS (status);
	else
		return -1;
#else
	return -1;
#endif
}