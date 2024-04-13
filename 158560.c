milliwait(struct itimerval *itval)
{
sigset_t sigmask;
sigset_t old_sigmask;

if (itval->it_value.tv_usec < 100 && itval->it_value.tv_sec == 0)
  return;
(void)sigemptyset(&sigmask);                           /* Empty mask */
(void)sigaddset(&sigmask, SIGALRM);                    /* Add SIGALRM */
(void)sigprocmask(SIG_BLOCK, &sigmask, &old_sigmask);  /* Block SIGALRM */
if (setitimer(ITIMER_REAL, itval, NULL) < 0)           /* Start timer */
  log_write(0, LOG_MAIN|LOG_PANIC_DIE,
    "setitimer() failed: %s", strerror(errno));
(void)sigfillset(&sigmask);                            /* All signals */
(void)sigdelset(&sigmask, SIGALRM);                    /* Remove SIGALRM */
(void)sigsuspend(&sigmask);                            /* Until SIGALRM */
(void)sigprocmask(SIG_SETMASK, &old_sigmask, NULL);    /* Restore mask */
}