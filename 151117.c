add_dbus_proxy_args (GPtrArray *argv_array,
                     GPtrArray *dbus_proxy_argv,
                     gboolean   enable_logging,
                     int        sync_fds[2],
                     const char *app_info_path,
                     GError   **error)
{
  char x = 'x';
  const char *proxy;
  g_autofree char *commandline = NULL;
  DbusProxySpawnData spawn_data;
  glnx_fd_close int app_info_fd = -1;
  glnx_fd_close int bwrap_args_fd = -1;

  if (dbus_proxy_argv->len == 0)
    return TRUE;

  if (sync_fds[0] == -1)
    {
      g_autofree char *fd_str = NULL;

      if (pipe (sync_fds) < 0)
        {
          g_set_error_literal (error, G_IO_ERROR, g_io_error_from_errno (errno),
                               _("Unable to create sync pipe"));
          return FALSE;
        }

      fd_str = g_strdup_printf ("%d", sync_fds[0]);
      add_args (argv_array, "--sync-fd", fd_str, NULL);
    }

  proxy = g_getenv ("FLATPAK_DBUSPROXY");
  if (proxy == NULL)
    proxy = DBUSPROXY;

  g_ptr_array_insert (dbus_proxy_argv, 0, g_strdup (proxy));
  g_ptr_array_insert (dbus_proxy_argv, 1, g_strdup_printf ("--fd=%d", sync_fds[1]));

  if (enable_logging)
    g_ptr_array_add (dbus_proxy_argv, g_strdup ("--log"));

  g_ptr_array_add (dbus_proxy_argv, NULL); /* NULL terminate */

  app_info_fd = open (app_info_path, O_RDONLY);
  if (app_info_fd == -1)
    {
      int errsv = errno;
      g_set_error (error, G_IO_ERROR, g_io_error_from_errno (errsv),
                   _("Failed to open app info file: %s"), g_strerror (errsv));
      return FALSE;
    }

  if (!prepend_bwrap_argv_wrapper (dbus_proxy_argv, app_info_fd, &bwrap_args_fd, error))
    return FALSE;

  commandline = g_strjoinv (" ", (char **) dbus_proxy_argv->pdata);
  g_debug ("Running '%s'", commandline);

  spawn_data.sync_fd = sync_fds[1];
  spawn_data.app_info_fd = app_info_fd;
  spawn_data.bwrap_args_fd = bwrap_args_fd;
  if (!g_spawn_async (NULL,
                      (char **) dbus_proxy_argv->pdata,
                      NULL,
                      G_SPAWN_SEARCH_PATH,
                      dbus_spawn_child_setup,
                      &spawn_data,
                      NULL, error))
    {
      close (sync_fds[0]);
      close (sync_fds[1]);
      return FALSE;
    }

  /* Sync with proxy, i.e. wait until its listening on the sockets */
  if (read (sync_fds[0], &x, 1) != 1)
    {
      g_set_error_literal (error, G_IO_ERROR, g_io_error_from_errno (errno),
                           _("Failed to sync with dbus proxy"));

      close (sync_fds[0]);
      close (sync_fds[1]);
      return FALSE;
    }

  return TRUE;
}