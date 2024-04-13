int imap_wait_keepalive (pid_t pid)
{
  struct sigaction oldalrm;
  struct sigaction act;
  sigset_t oldmask;
  int rc;

  short imap_passive = option (OPTIMAPPASSIVE);

  set_option (OPTIMAPPASSIVE);
  set_option (OPTKEEPQUIET);

  sigprocmask (SIG_SETMASK, NULL, &oldmask);

  sigemptyset (&act.sa_mask);
  act.sa_handler = alrm_handler;
#ifdef SA_INTERRUPT
  act.sa_flags = SA_INTERRUPT;
#else
  act.sa_flags = 0;
#endif

  sigaction (SIGALRM, &act, &oldalrm);

  alarm (ImapKeepalive);
  while (waitpid (pid, &rc, 0) < 0 && errno == EINTR)
  {
    alarm (0); /* cancel a possibly pending alarm */
    imap_keepalive ();
    alarm (ImapKeepalive);
  }

  alarm (0);	/* cancel a possibly pending alarm */

  sigaction (SIGALRM, &oldalrm, NULL);
  sigprocmask (SIG_SETMASK, &oldmask, NULL);

  unset_option (OPTKEEPQUIET);
  if (!imap_passive)
    unset_option (OPTIMAPPASSIVE);

  return rc;
}