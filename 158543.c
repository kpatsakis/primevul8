open_msglog_file(uschar *filename, int mode, uschar **error)
{
int fd, i;

for (i = 2; i > 0; i--)
  {
  fd = Uopen(filename,
#ifdef O_CLOEXEC
    O_CLOEXEC |
#endif
#ifdef O_NOFOLLOW
    O_NOFOLLOW |
#endif
		O_WRONLY|O_APPEND|O_CREAT, mode);
  if (fd >= 0)
    {
    /* Set the close-on-exec flag and change the owner to the exim uid/gid (this
    function is called as root). Double check the mode, because the group setting
    doesn't always get set automatically. */

#ifndef O_CLOEXEC
    (void)fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
#endif
    if (fchown(fd, exim_uid, exim_gid) < 0)
      {
      *error = US"chown";
      return -1;
      }
    if (fchmod(fd, mode) < 0)
      {
      *error = US"chmod";
      return -1;
      }
    return fd;
    }
  if (errno != ENOENT)
    break;

  (void)directory_make(spool_directory,
			spool_sname(US"msglog", message_subdir),
			MSGLOG_DIRECTORY_MODE, TRUE);
  }

*error = US"create";
return -1;
}