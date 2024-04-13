sigalrm_handler(int sig)
{
sig = sig;            /* Keep picky compilers happy */
sigalrm_seen = TRUE;
}