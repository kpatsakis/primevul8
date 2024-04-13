log_close_all(void)
{
if (mainlogfd >= 0)
  { (void)close(mainlogfd); mainlogfd = -1; }
if (rejectlogfd >= 0)
  { (void)close(rejectlogfd); rejectlogfd = -1; }
closelog();
syslog_open = FALSE;
}