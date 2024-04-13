usr1_handler(int sig)
{
int fd;

os_restarting_signal(sig, usr1_handler);

if ((fd = Uopen(process_log_path, O_APPEND|O_WRONLY, LOG_MODE)) < 0)
  {
  /* If we are already running as the Exim user, try to create it in the
  current process (assuming spool_directory exists). Otherwise, if we are
  root, do the creation in an exim:exim subprocess. */

  int euid = geteuid();
  if (euid == exim_uid)
    fd = Uopen(process_log_path, O_CREAT|O_APPEND|O_WRONLY, LOG_MODE);
  else if (euid == root_uid)
    fd = log_create_as_exim(process_log_path);
  }

/* If we are neither exim nor root, or if we failed to create the log file,
give up. There is not much useful we can do with errors, since we don't want
to disrupt whatever is going on outside the signal handler. */

if (fd < 0) return;

(void)write(fd, process_info, process_info_len);
(void)close(fd);
}