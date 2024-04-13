exim_nullstd(void)
{
int i;
int devnull = -1;
struct stat statbuf;
for (i = 0; i <= 2; i++)
  {
  if (fstat(i, &statbuf) < 0 && errno == EBADF)
    {
    if (devnull < 0) devnull = open("/dev/null", O_RDWR);
    if (devnull < 0) log_write(0, LOG_MAIN|LOG_PANIC_DIE, "%s",
      string_open_failed(errno, "/dev/null"));
    if (devnull != i) (void)dup2(devnull, i);
    }
  }
if (devnull > 2) (void)close(devnull);
}