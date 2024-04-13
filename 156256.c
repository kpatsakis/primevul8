mm_answer_pty_cleanup(int socket, Buffer *m)
{
	Session *s;
	char *tty;

	debug3("%s entering", __func__);

	tty = buffer_get_string(m, NULL);
	if ((s = session_by_tty(tty)) != NULL)
		mm_session_close(s);
	buffer_clear(m);
	xfree(tty);
	return (0);
}