gpg_ctx_op_start (struct _GpgCtx *gpg,
                  GError **error)
{
#ifndef G_OS_WIN32
	gchar *status_fd = NULL, *passwd_fd = NULL;
	gint i, maxfd, errnosave, fds[10];
	GPtrArray *argv;
	gint flags;

	for (i = 0; i < 10; i++)
		fds[i] = -1;

	maxfd = gpg->need_passwd ? 10 : 8;
	for (i = 0; i < maxfd; i += 2) {
		if (pipe (fds + i) == -1)
			goto exception;
	}

	argv = gpg_ctx_get_argv (gpg, fds[7], &status_fd, fds[8], &passwd_fd);

	if (!(gpg->pid = fork ())) {
		/* child process */

		if ((dup2 (fds[0], STDIN_FILENO) < 0 ) ||
		    (dup2 (fds[3], STDOUT_FILENO) < 0 ) ||
		    (dup2 (fds[5], STDERR_FILENO) < 0 )) {
			_exit (255);
		}

		/* Dissociate from camel's controlling terminal so
		 * that gpg won't be able to read from it.
		 */
		setsid ();

		maxfd = sysconf (_SC_OPEN_MAX);
		/* Loop over all fds. */
		for (i = 3; i < maxfd; i++) {
			/* don't close the status-fd or passwd-fd */
			if (i != fds[7] && i != fds[8])
				fcntl (i, F_SETFD, FD_CLOEXEC);
		}

		/* run gpg */
		execvp (gpg_ctx_get_executable_name (), (gchar **) argv->pdata);
		_exit (255);
	} else if (gpg->pid < 0) {
		g_ptr_array_free (argv, TRUE);
		g_free (status_fd);
		g_free (passwd_fd);
		goto exception;
	}

	g_ptr_array_free (argv, TRUE);
	g_free (status_fd);
	g_free (passwd_fd);

	/* Parent */
	close (fds[0]);
	gpg->stdin_fd = fds[1];
	gpg->stdout_fd = fds[2];
	close (fds[3]);
	gpg->stderr_fd = fds[4];
	close (fds[5]);
	gpg->status_fd = fds[6];
	close (fds[7]);

	if (gpg->need_passwd) {
		close (fds[8]);
		gpg->passwd_fd = fds[9];
		flags = fcntl (gpg->passwd_fd, F_GETFL);
		fcntl (gpg->passwd_fd, F_SETFL, flags | O_NONBLOCK);
	}

	flags = fcntl (gpg->stdin_fd, F_GETFL);
	fcntl (gpg->stdin_fd, F_SETFL, flags | O_NONBLOCK);

	flags = fcntl (gpg->stdout_fd, F_GETFL);
	fcntl (gpg->stdout_fd, F_SETFL, flags | O_NONBLOCK);

	flags = fcntl (gpg->stderr_fd, F_GETFL);
	fcntl (gpg->stderr_fd, F_SETFL, flags | O_NONBLOCK);

	flags = fcntl (gpg->status_fd, F_GETFL);
	fcntl (gpg->status_fd, F_SETFL, flags | O_NONBLOCK);

	return TRUE;

exception:

	errnosave = errno;

	for (i = 0; i < 10; i++) {
		if (fds[i] != -1)
			close (fds[i]);
	}

	errno = errnosave;
#else
	/* FIXME: Port me */
	g_warning ("%s: Not implemented", G_STRFUNC);

	errno = EINVAL;
#endif

	if (errno != 0)
		g_set_error (
			error, G_IO_ERROR,
			g_io_error_from_errno (errno),
			_("Failed to execute gpg: %s"),
			g_strerror (errno));
	else
		g_set_error (
			error, CAMEL_ERROR, CAMEL_ERROR_GENERIC,
			_("Failed to execute gpg: %s"), _("Unknown"));

	return FALSE;
}