sigalrm_handler(int sig)
{
sig = sig;      /* Keep picky compilers happy */
sigalrm_seen = TRUE;
os_non_restarting_signal(SIGALRM, sigalrm_handler);
}