mm_session_close(Session *s)
{
	debug3("%s: session %d pid %d", __func__, s->self, s->pid);
	if (s->ttyfd != -1) {
		debug3("%s: tty %s ptyfd %d",  __func__, s->tty, s->ptyfd);
		fatal_remove_cleanup(session_pty_cleanup2, (void *)s);
		session_pty_cleanup2(s);
	}
	s->used = 0;
}