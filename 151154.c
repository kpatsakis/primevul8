create_tmp_fd (const char *contents,
               gssize      length,
               GError    **error)
{
  char template[] = "/tmp/tmp_fd_XXXXXX";
  int fd;

  if (length < 0)
    length = strlen (contents);

  fd = g_mkstemp (template);
  if (fd < 0)
    {
      g_set_error_literal (error, G_IO_ERROR, g_io_error_from_errno (errno),
                           _("Failed to create temporary file"));
      return -1;
    }

  if (unlink (template) != 0)
    {
      g_set_error_literal (error, G_IO_ERROR, g_io_error_from_errno (errno),
                           _("Failed to unlink temporary file"));
      close (fd);
      return -1;
    }

  while (length > 0)
    {
      gssize s;

      s = write (fd, contents, length);

      if (s < 0)
        {
          int saved_errno = errno;
          if (saved_errno == EINTR)
            continue;

          g_set_error_literal (error, G_IO_ERROR, g_io_error_from_errno (saved_errno),
                               _("Failed to write to temporary file"));
          close (fd);
          return -1;
        }

      g_assert (s <= length);

      contents += s;
      length -= s;
    }

  lseek (fd, 0, SEEK_SET);

  return fd;
}