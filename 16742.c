gpg_ctx_op_step (struct _GpgCtx *gpg,
                 GCancellable *cancellable,
                 GError **error)
{
#ifndef G_OS_WIN32
	GPollFD polls[6];
	gint status, i;
	gboolean read_data = FALSE, wrote_data = FALSE;

	for (i = 0; i < 6; i++) {
		polls[i].fd = -1;
		polls[i].events = 0;
	}

	if (!gpg->seen_eof1) {
		polls[0].fd = gpg->stdout_fd;
		polls[0].events = G_IO_IN;
	}

	if (!gpg->seen_eof2) {
		polls[1].fd = gpg->stderr_fd;
		polls[1].events = G_IO_IN;
	}

	if (!gpg->complete) {
		polls[2].fd = gpg->status_fd;
		polls[2].events = G_IO_IN;
	}

	polls[3].fd = gpg->stdin_fd;
	polls[3].events = G_IO_OUT;
	polls[4].fd = gpg->passwd_fd;
	polls[4].events = G_IO_OUT;
	polls[5].fd = g_cancellable_get_fd (cancellable);
	polls[5].events = G_IO_IN;

	do {
		for (i = 0; i < 6; i++)
			polls[i].revents = 0;
		status = g_poll (polls, 6, 30 * 1000);
	} while (status == -1 && errno == EINTR);

	if (status == 0)
		return 0;	/* timed out */
	else if (status == -1)
		goto exception;

	if ((polls[5].revents & G_IO_IN) &&
		g_cancellable_set_error_if_cancelled (cancellable, error)) {

		gpg_ctx_op_cancel (gpg);
		return -1;
	}

	/* Test each and every file descriptor to see if it's 'ready',
	 * and if so - do what we can with it and then drop through to
	 * the next file descriptor and so on until we've done what we
	 * can to all of them. If one fails along the way, return
	 * -1. */

	if (polls[2].revents & (G_IO_IN | G_IO_HUP)) {
		/* read the status message and decide what to do... */
		gchar buffer[4096];
		gssize nread;

		d (printf ("reading from gpg's status-fd...\n"));

		do {
			nread = read (gpg->status_fd, buffer, sizeof (buffer));
		} while (nread == -1 && (errno == EINTR || errno == EAGAIN));
		if (nread == -1)
			goto exception;

		if (nread > 0) {
			status_backup (gpg, buffer, nread);

			if (gpg_ctx_parse_status (gpg, error) == -1)
				return -1;
		} else {
			gpg->complete = TRUE;
		}
	}

	if ((polls[0].revents & (G_IO_IN | G_IO_HUP)) && gpg->ostream) {
		gchar buffer[4096];
		gssize nread;

		d (printf ("reading gpg's stdout...\n"));

		do {
			nread = read (gpg->stdout_fd, buffer, sizeof (buffer));
		} while (nread == -1 && (errno == EINTR || errno == EAGAIN));
		if (nread == -1)
			goto exception;

		if (nread > 0) {
			gsize written = camel_stream_write (
				gpg->ostream, buffer, (gsize)
				nread, cancellable, error);
			if (written != nread)
				return -1;
		} else {
			gpg->seen_eof1 = TRUE;
		}

		read_data = TRUE;
	}

	if (polls[1].revents & (G_IO_IN | G_IO_HUP)) {
		gchar buffer[4096];
		gssize nread;

		d (printf ("reading gpg's stderr...\n"));

		do {
			nread = read (gpg->stderr_fd, buffer, sizeof (buffer));
		} while (nread == -1 && (errno == EINTR || errno == EAGAIN));
		if (nread == -1)
			goto exception;

		if (nread > 0) {
			camel_stream_write (
				gpg->diagnostics, buffer,
				nread, cancellable, error);
		} else {
			gpg->seen_eof2 = TRUE;
		}
	}

	if ((polls[4].revents & (G_IO_OUT | G_IO_HUP)) && gpg->need_passwd && gpg->send_passwd) {
		gssize w, nwritten = 0;
		gsize n;

		d (printf ("sending gpg our passphrase...\n"));

		/* send the passphrase to gpg */
		n = strlen (gpg->passwd);
		do {
			do {
				w = write (gpg->passwd_fd, gpg->passwd + nwritten, n - nwritten);
			} while (w == -1 && (errno == EINTR || errno == EAGAIN));

			if (w > 0)
				nwritten += w;
		} while (nwritten < n && w != -1);

		/* zero and free our passwd buffer */
		memset (gpg->passwd, 0, n);
		g_free (gpg->passwd);
		gpg->passwd = NULL;

		if (w == -1)
			goto exception;

		gpg->send_passwd = FALSE;
	}

	if ((polls[3].revents & (G_IO_OUT | G_IO_HUP)) && gpg->istream) {
		gchar buffer[4096];
		gssize nread;

		d (printf ("writing to gpg's stdin...\n"));

		/* write our stream to gpg's stdin */
		nread = camel_stream_read (
			gpg->istream, buffer,
			sizeof (buffer), cancellable, NULL);
		if (nread > 0) {
			gssize w, nwritten = 0;

			do {
				do {
					w = write (gpg->stdin_fd, buffer + nwritten, nread - nwritten);
				} while (w == -1 && (errno == EINTR || errno == EAGAIN));

				if (w > 0)
					nwritten += w;
			} while (nwritten < nread && w != -1);

			if (w == -1)
				goto exception;

			d (printf ("wrote %d (out of %d) bytes to gpg's stdin\n", nwritten, nread));
			wrote_data = TRUE;
		}

		if (camel_stream_eos (gpg->istream)) {
			d (printf ("closing gpg's stdin\n"));
			close (gpg->stdin_fd);
			gpg->stdin_fd = -1;
		}
	}

	if (gpg->need_id && !gpg->processing && !read_data && !wrote_data) {
		/* do not ask more than hundred times per second when looking for a pass phrase,
		 * in case user has the use-agent set, it'll not use the all CPU when
		 * agent is asking for a pass phrase, instead of us */
		g_usleep (G_USEC_PER_SEC / 100);
	}

	return 0;

 exception:
	/* always called on an i/o error */
	g_set_error (
		error, G_IO_ERROR,
		g_io_error_from_errno (errno),
		_("Failed to execute gpg: %s"), g_strerror (errno));
	gpg_ctx_op_cancel (gpg);
#endif
	return -1;
}