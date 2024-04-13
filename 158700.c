log_create_as_exim(uschar *name)
{
pid_t pid = fork();
int status = 1;
int fd = -1;

/* In the subprocess, change uid/gid and do the creation. Return 0 from the
subprocess on success. If we don't check for setuid failures, then the file
can be created as root, so vulnerabilities which cause setuid to fail mean
that the Exim user can use symlinks to cause a file to be opened/created as
root. We always open for append, so can't nuke existing content but it would
still be Rather Bad. */

if (pid == 0)
  {
  if (setgid(exim_gid) < 0)
    die(US"exim: setgid for log-file creation failed, aborting",
      US"Unexpected log failure, please try later");
  if (setuid(exim_uid) < 0)
    die(US"exim: setuid for log-file creation failed, aborting",
      US"Unexpected log failure, please try later");
  _exit((log_create(name) < 0)? 1 : 0);
  }

/* If we created a subprocess, wait for it. If it succeeded, try the open. */

while (pid > 0 && waitpid(pid, &status, 0) != pid);
if (status == 0) fd = Uopen(name,
#ifdef O_CLOEXEC
			O_CLOEXEC |
#endif
		       	O_APPEND|O_WRONLY, LOG_MODE);

/* If we failed to create a subprocess, we are in a bad way. We return
with fd still < 0, and errno set, letting the caller handle the error. */

return fd;
}