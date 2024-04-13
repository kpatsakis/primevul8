open_log(int *fd, int type, uschar *tag)
{
uid_t euid;
BOOL ok, ok2;
uschar buffer[LOG_NAME_SIZE];

/* The names of the log files are controlled by file_path. The panic log is
written to the same directory as the main and reject logs, but its name does
not have a datestamp. The use of datestamps is indicated by %D/%M in file_path.
When opening the panic log, if %D or %M is present, we remove the datestamp
from the generated name; if it is at the start, remove a following
non-alphanumeric character as well; otherwise, remove a preceding
non-alphanumeric character. This is definitely kludgy, but it sort of does what
people want, I hope. */

ok = string_format(buffer, sizeof(buffer), CS file_path, log_names[type]);

/* Save the name of the mainlog for rollover processing. Without a datestamp,
it gets statted to see if it has been cycled. With a datestamp, the datestamp
will be compared. The static slot for saving it is the same size as buffer,
and the text has been checked above to fit, so this use of strcpy() is OK. */

if (type == lt_main && string_datestamp_offset >= 0)
  {
  Ustrcpy(mainlog_name, buffer);
  mainlog_datestamp = mainlog_name + string_datestamp_offset;
  }

/* Ditto for the reject log */

else if (type == lt_reject && string_datestamp_offset >= 0)
  {
  Ustrcpy(rejectlog_name, buffer);
  rejectlog_datestamp = rejectlog_name + string_datestamp_offset;
  }

/* and deal with the debug log (which keeps the datestamp, but does not
update it) */

else if (type == lt_debug)
  {
  Ustrcpy(debuglog_name, buffer);
  if (tag)
    {
    /* this won't change the offset of the datestamp */
    ok2 = string_format(buffer, sizeof(buffer), "%s%s",
      debuglog_name, tag);
    if (ok2)
      Ustrcpy(debuglog_name, buffer);
    }
  }

/* Remove any datestamp if this is the panic log. This is rare, so there's no
need to optimize getting the datestamp length. We remove one non-alphanumeric
char afterwards if at the start, otherwise one before. */

else if (string_datestamp_offset >= 0)
  {
  uschar * from = buffer + string_datestamp_offset;
  uschar * to = from + string_datestamp_length;

  if (from == buffer || from[-1] == '/')
    {
    if (!isalnum(*to)) to++;
    }
  else
    if (!isalnum(from[-1])) from--;

  /* This copy is ok, because we know that to is a substring of from. But
  due to overlap we must use memmove() not Ustrcpy(). */
  memmove(from, to, Ustrlen(to)+1);
  }

/* If the file name is too long, it is an unrecoverable disaster */

if (!ok)
  die(US"exim: log file path too long: aborting",
      US"Logging failure; please try later");

/* We now have the file name. Try to open an existing file. After a successful
open, arrange for automatic closure on exec(), and then return. */

*fd = Uopen(buffer,
#ifdef O_CLOEXEC
		O_CLOEXEC |
#endif
	       	O_APPEND|O_WRONLY, LOG_MODE);

if (*fd >= 0)
  {
#ifndef O_CLOEXEC
  (void)fcntl(*fd, F_SETFD, fcntl(*fd, F_GETFD) | FD_CLOEXEC);
#endif
  return;
  }

/* Open was not successful: try creating the file. If this is a root process,
we must do the creating in a subprocess set to exim:exim in order to ensure
that the file is created with the right ownership. Otherwise, there can be a
race if another Exim process is trying to write to the log at the same time.
The use of SIGUSR1 by the exiwhat utility can provoke a lot of simultaneous
writing. */

euid = geteuid();

/* If we are already running as the Exim user (even if that user is root),
we can go ahead and create in the current process. */

if (euid == exim_uid) *fd = log_create(buffer);

/* Otherwise, if we are root, do the creation in an exim:exim subprocess. If we
are neither exim nor root, creation is not attempted. */

else if (euid == root_uid) *fd = log_create_as_exim(buffer);

/* If we now have an open file, set the close-on-exec flag and return. */

if (*fd >= 0)
  {
#ifndef O_CLOEXEC
  (void)fcntl(*fd, F_SETFD, fcntl(*fd, F_GETFD) | FD_CLOEXEC);
#endif
  return;
  }

/* Creation failed. There are some circumstances in which we get here when
the effective uid is not root or exim, which is the problem. (For example, a
non-setuid binary with log_arguments set, called in certain ways.) Rather than
just bombing out, force the log to stderr and carry on if stderr is available.
*/

if (euid != root_uid && euid != exim_uid && log_stderr != NULL)
  {
  *fd = fileno(log_stderr);
  return;
  }

/* Otherwise this is a disaster. This call is deliberately ONLY to the panic
log. If possible, save a copy of the original line that was being logged. If we
are recursing (can't open the panic log either), the pointer will already be
set. */

if (!panic_save_buffer)
  if ((panic_save_buffer = US malloc(LOG_BUFFER_SIZE)))
    memcpy(panic_save_buffer, log_buffer, LOG_BUFFER_SIZE);

log_write(0, LOG_PANIC_DIE, "Cannot open %s log file \"%s\": %s: "
  "euid=%d egid=%d", log_names[type], buffer, strerror(errno), euid, getegid());
/* Never returns */
}