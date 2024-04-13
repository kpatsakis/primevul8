add_monitor_path_args (gboolean use_session_helper,
                       GPtrArray *argv_array)
{
  g_autoptr(AutoFlatpakSessionHelper) session_helper = NULL;
  g_autofree char *monitor_path = NULL;

  if (use_session_helper)
    {
      session_helper =
        flatpak_session_helper_proxy_new_for_bus_sync (G_BUS_TYPE_SESSION,
                                                       G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES | G_DBUS_PROXY_FLAGS_DO_NOT_CONNECT_SIGNALS,
                                                       "org.freedesktop.Flatpak",
                                                       "/org/freedesktop/Flatpak/SessionHelper",
                                                       NULL, NULL);
    }

  if (session_helper &&
      flatpak_session_helper_call_request_monitor_sync (session_helper,
                                                        &monitor_path,
                                                        NULL, NULL))
    {
      add_args (argv_array,
                "--bind", monitor_path, "/run/host/monitor",
                NULL);
      add_args (argv_array,
                "--symlink", "/run/host/monitor/localtime", "/etc/localtime",
                NULL);
      add_args (argv_array,
                "--symlink", "/run/host/monitor/resolv.conf", "/etc/resolv.conf",
                NULL);
    }
  else
    {
      /* /etc/localtime and /etc/resolv.conf can not exist (or be symlinks to
       * non-existing targets), in which case we don't want to attempt to create
       * bogus symlinks or bind mounts, as that will cause flatpak run to fail.
       */
      if (g_file_test ("/etc/localtime", G_FILE_TEST_EXISTS))
        {
          char localtime[PATH_MAX + 1];
          ssize_t symlink_size;

          symlink_size = readlink ("/etc/localtime", localtime, sizeof (localtime) - 1);
          if (symlink_size > 0)
            {
              localtime[symlink_size] = 0;
              add_args (argv_array,
                        "--symlink", localtime, "/etc/localtime",
                        NULL);
            }
          else
            {
              add_args (argv_array,
                        "--bind", "/etc/localtime", "/etc/localtime",
                        NULL);
            }
        }

      if (g_file_test ("/etc/resolv.conf", G_FILE_TEST_EXISTS))
        {
          add_args (argv_array,
                    "--bind", "/etc/resolv.conf", "/etc/resolv.conf",
                    NULL);
        }
    }
}