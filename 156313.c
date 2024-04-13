mm_answer_pty(int socket, Buffer *m)
{
	extern struct monitor *pmonitor;
	Session *s;
	int res, fd0;

	debug3("%s entering", __func__);

	buffer_clear(m);
	s = session_new();
	if (s == NULL)
		goto error;
	s->authctxt = authctxt;
	s->pw = authctxt->pw;
	s->pid = pmonitor->m_pid;
	res = pty_allocate(&s->ptyfd, &s->ttyfd, s->tty, sizeof(s->tty));
	if (res == 0)
		goto error;
	fatal_add_cleanup(session_pty_cleanup2, (void *)s);
	pty_setowner(authctxt->pw, s->tty);

	buffer_put_int(m, 1);
	buffer_put_cstring(m, s->tty);
	mm_request_send(socket, MONITOR_ANS_PTY, m);

	mm_send_fd(socket, s->ptyfd);
	mm_send_fd(socket, s->ttyfd);

	/* We need to trick ttyslot */
	if (dup2(s->ttyfd, 0) == -1)
		fatal("%s: dup2", __func__);

	mm_record_login(s, authctxt->pw);

	/* Now we can close the file descriptor again */
	close(0);

	/* make sure nothing uses fd 0 */
	if ((fd0 = open(_PATH_DEVNULL, O_RDONLY)) < 0)
		fatal("%s: open(/dev/null): %s", __func__, strerror(errno));
	if (fd0 != 0)
		error("%s: fd0 %d != 0", __func__, fd0);

	/* slave is not needed */
	close(s->ttyfd);
	s->ttyfd = s->ptyfd;
	/* no need to dup() because nobody closes ptyfd */
	s->ptymaster = s->ptyfd;

	debug3("%s: tty %s ptyfd %d",  __func__, s->tty, s->ttyfd);

	return (0);

 error:
	if (s != NULL)
		mm_session_close(s);
	buffer_put_int(m, 0);
	mm_request_send(socket, MONITOR_ANS_PTY, m);
	return (0);
}