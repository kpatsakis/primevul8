flatpak_run_add_environment_args (GPtrArray      *argv_array,
                                  GArray         *fd_array,
                                  char         ***envp_p,
                                  GPtrArray      *session_bus_proxy_argv,
                                  GPtrArray      *system_bus_proxy_argv,
                                  const char     *app_id,
                                  FlatpakContext *context,
                                  GFile          *app_id_dir)
{
  GHashTableIter iter;
  gpointer key, value;
  gboolean unrestricted_session_bus;
  gboolean unrestricted_system_bus;
  gboolean home_access = FALSE;
  GString *xdg_dirs_conf = NULL;
  FlatpakFilesystemMode fs_mode, home_mode;
  g_autoptr(GFile) user_flatpak_dir = NULL;
  g_autoptr(GHashTable) fs_paths = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);

  if ((context->shares & FLATPAK_CONTEXT_SHARED_IPC) == 0)
    {
      g_debug ("Disallowing ipc access");
      add_args (argv_array, "--unshare-ipc", NULL);
    }

  if ((context->shares & FLATPAK_CONTEXT_SHARED_NETWORK) == 0)
    {
      g_debug ("Disallowing network access");
      add_args (argv_array, "--unshare-net", NULL);
    }

  if (context->devices & FLATPAK_CONTEXT_DEVICE_ALL)
    {
      add_args (argv_array,
                "--dev-bind", "/dev", "/dev",
                NULL);
    }
  else
    {
      add_args (argv_array,
                "--dev", "/dev",
                NULL);
      if (context->devices & FLATPAK_CONTEXT_DEVICE_DRI)
        {
          g_debug ("Allowing dri access");
          if (g_file_test ("/dev/dri", G_FILE_TEST_IS_DIR))
            add_args (argv_array, "--dev-bind", "/dev/dri", "/dev/dri", NULL);
          if (g_file_test ("/dev/nvidiactl", G_FILE_TEST_EXISTS))
            {
              add_args (argv_array,
                        "--dev-bind", "/dev/nvidiactl", "/dev/nvidiactl",
                        "--dev-bind", "/dev/nvidia0", "/dev/nvidia0",
                        NULL);
            }
        }
      if (context->devices & FLATPAK_CONTEXT_DEVICE_KVM)
        {
          g_debug ("Allowing kvm access");
          if (g_file_test ("/dev/kvm", G_FILE_TEST_EXISTS))
            add_args (argv_array, "--dev-bind", "/dev/kvm", "/dev/kvm", NULL);
        }
    }

  fs_mode = (FlatpakFilesystemMode) g_hash_table_lookup (context->filesystems, "host");
  if (fs_mode != 0)
    {
      DIR *dir;
      struct dirent *dirent;

      g_debug ("Allowing host-fs access");
      home_access = TRUE;

      /* Bind mount most dirs in / into the new root */
      dir = opendir ("/");
      if (dir != NULL)
        {
          while ((dirent = readdir (dir)))
            {
              g_autofree char *path = NULL;

              if (g_strv_contains (dont_mount_in_root, dirent->d_name))
                continue;

              path = g_build_filename ("/", dirent->d_name, NULL);
              add_expose_path (fs_paths, fs_mode, path);
            }
          closedir (dir);
        }
      add_expose_path (fs_paths, fs_mode, "/run/media");
    }

  home_mode = (FlatpakFilesystemMode) g_hash_table_lookup (context->filesystems, "home");
  if (home_mode != 0)
    {
      g_debug ("Allowing homedir access");
      home_access = TRUE;

      add_expose_path (fs_paths, MAX (home_mode, fs_mode), g_get_home_dir ());
    }

  if (!home_access)
    {
      /* Enable persistent mapping only if no access to real home dir */

      g_hash_table_iter_init (&iter, context->persistent);
      while (g_hash_table_iter_next (&iter, &key, NULL))
        {
          const char *persist = key;
          g_autofree char *src = g_build_filename (g_get_home_dir (), ".var/app", app_id, persist, NULL);
          g_autofree char *dest = g_build_filename (g_get_home_dir (), persist, NULL);

          g_mkdir_with_parents (src, 0755);

          add_args (argv_array,
                    "--bind", src, dest,
                    NULL);
        }
    }

  {
    g_autofree char *run_user_app_dst = g_strdup_printf ("/run/user/%d/app/%s", getuid (), app_id);
    g_autofree char *run_user_app_src = g_build_filename (g_get_user_runtime_dir (), "app", app_id, NULL);

    if (glnx_shutil_mkdir_p_at (AT_FDCWD,
                                run_user_app_src,
                                0700,
                                NULL,
                                NULL))
        add_args (argv_array,
                  "--bind", run_user_app_src, run_user_app_dst,
                  NULL);
  }

  g_hash_table_iter_init (&iter, context->filesystems);
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      const char *filesystem = key;
      FlatpakFilesystemMode mode = GPOINTER_TO_INT (value);

      if (value == NULL ||
          strcmp (filesystem, "host") == 0 ||
          strcmp (filesystem, "home") == 0)
        continue;

      if (g_str_has_prefix (filesystem, "xdg-"))
        {
          const char *path, *rest = NULL;
          const char *config_key = NULL;
          g_autofree char *subpath = NULL;

          if (!get_xdg_user_dir_from_string (filesystem, &config_key, &rest, &path))
            {
              g_warning ("Unsupported xdg dir %s\n", filesystem);
              continue;
            }

          if (path == NULL)
            continue; /* Unconfigured, ignore */

          if (strcmp (path, g_get_home_dir ()) == 0)
            {
              /* xdg-user-dirs sets disabled dirs to $HOME, and its in general not a good
                 idea to set full access to $HOME other than explicitly, so we ignore
                 these */
              g_debug ("Xdg dir %s is $HOME (i.e. disabled), ignoring\n", filesystem);
              continue;
            }

          subpath = g_build_filename (path, rest, NULL);

          if (mode == FLATPAK_FILESYSTEM_MODE_CREATE)
            g_mkdir_with_parents (subpath, 0755);

          if (g_file_test (subpath, G_FILE_TEST_EXISTS))
            {
              if (xdg_dirs_conf == NULL)
                xdg_dirs_conf = g_string_new ("");

              if (config_key)
                g_string_append_printf (xdg_dirs_conf, "%s=\"%s\"\n",
                                        config_key, path);

              add_expose_path (fs_paths, mode, subpath);
            }
        }
      else if (g_str_has_prefix (filesystem, "~/"))
        {
          g_autofree char *path = NULL;

          path = g_build_filename (g_get_home_dir (), filesystem + 2, NULL);

          if (mode == FLATPAK_FILESYSTEM_MODE_CREATE)
            g_mkdir_with_parents (path, 0755);

          if (g_file_test (path, G_FILE_TEST_EXISTS))
            add_expose_path (fs_paths, mode, path);
        }
      else if (g_str_has_prefix (filesystem, "/"))
        {
          if (mode == FLATPAK_FILESYSTEM_MODE_CREATE)
            g_mkdir_with_parents (filesystem, 0755);

          if (g_file_test (filesystem, G_FILE_TEST_EXISTS))
            add_expose_path (fs_paths, mode, filesystem);
        }
      else
        {
          g_warning ("Unexpected filesystem arg %s\n", filesystem);
        }
    }

  if (app_id_dir)
    {
      g_autoptr(GFile) apps_dir = g_file_get_parent (app_id_dir);
      /* Hide the .var/app dir by default (unless explicitly made visible) */
      add_hide_path (fs_paths, flatpak_file_get_path_cached (apps_dir));
      /* But let the app write to the per-app dir in it */
      add_expose_path (fs_paths, FLATPAK_FILESYSTEM_MODE_READ_WRITE,
                       flatpak_file_get_path_cached (app_id_dir));
    }

  /* Hide the flatpak dir by default (unless explicitly made visible) */
  user_flatpak_dir = flatpak_get_user_base_dir_location ();
  add_hide_path (fs_paths, flatpak_file_get_path_cached (user_flatpak_dir));

  /* This actually outputs the args for the hide/expose operations above */
  add_file_args (argv_array, fs_paths);

  /* Special case subdirectories of the cache, config and data xdg dirs.
   * If these are accessible explicilty, in a read-write fashion, then
   * we bind-mount these in the app-id dir. This allows applications to
   * explicitly opt out of keeping some config/cache/data in the
   * app-specific directory.
   */
  if (app_id_dir)
    {
      g_hash_table_iter_init (&iter, context->filesystems);
      while (g_hash_table_iter_next (&iter, &key, &value))
        {
          const char *filesystem = key;
          FlatpakFilesystemMode mode = GPOINTER_TO_INT (value);
          g_autofree char *xdg_path = NULL;
          const char *rest, *where;

          xdg_path = get_xdg_dir_from_string (filesystem, &rest, &where);

          if (xdg_path != NULL && *rest != 0 &&
              mode > FLATPAK_FILESYSTEM_MODE_READ_WRITE)
            {
              g_autoptr(GFile) app_version = g_file_get_child (app_id_dir, where);
              g_autoptr(GFile) app_version_subdir = g_file_resolve_relative_path (app_version, rest);

              if (g_file_test (xdg_path, G_FILE_TEST_IS_DIR))
                {
                  g_autofree char *xdg_path_in_app = g_file_get_path (app_version_subdir);
                  g_mkdir_with_parents (xdg_path_in_app, 0755);
                  add_args (argv_array,
                            "--bind", xdg_path, xdg_path_in_app,
                            NULL);
                }
            }
        }
    }

  if (home_access  && app_id_dir != NULL)
    {
      g_autofree char *src_path = g_build_filename (g_get_user_config_dir (),
                                                    "user-dirs.dirs",
                                                    NULL);
      g_autofree char *path = g_build_filename (flatpak_file_get_path_cached (app_id_dir),
                                                "config/user-dirs.dirs", NULL);
      if (g_file_test (src_path, G_FILE_TEST_EXISTS))
        add_args (argv_array,
                  "--ro-bind", src_path, path,
                  NULL);
    }
  else if (xdg_dirs_conf != NULL && app_id_dir != NULL)
    {
      g_autofree char *tmp_path = NULL;
      g_autofree char *path = NULL;
      int fd;

      fd = g_file_open_tmp ("flatpak-user-dir-XXXXXX.dirs", &tmp_path, NULL);
      if (fd >= 0)
        {
          close (fd);
          if (g_file_set_contents (tmp_path, xdg_dirs_conf->str, xdg_dirs_conf->len, NULL))
            {
              int tmp_fd = open (tmp_path, O_RDONLY);
              unlink (tmp_path);
              if (tmp_fd != -1)
                {
                  g_autofree char *tmp_fd_str = g_strdup_printf ("%d", tmp_fd);
                  if (fd_array)
                    g_array_append_val (fd_array, tmp_fd);
                  path = g_build_filename (flatpak_file_get_path_cached (app_id_dir),
                                           "config/user-dirs.dirs", NULL);

                  add_args (argv_array, "--file", tmp_fd_str, path, NULL);
                }
            }
        }
      g_string_free (xdg_dirs_conf, TRUE);
    }

  flatpak_run_add_x11_args (argv_array, fd_array, envp_p,
                            (context->sockets & FLATPAK_CONTEXT_SOCKET_X11) != 0);

  if (context->sockets & FLATPAK_CONTEXT_SOCKET_WAYLAND)
    {
      g_debug ("Allowing wayland access");
      flatpak_run_add_wayland_args (argv_array, envp_p);
    }

  if (context->sockets & FLATPAK_CONTEXT_SOCKET_PULSEAUDIO)
    {
      g_debug ("Allowing pulseaudio access");
      flatpak_run_add_pulseaudio_args (argv_array, fd_array, envp_p);
    }

  unrestricted_session_bus = (context->sockets & FLATPAK_CONTEXT_SOCKET_SESSION_BUS) != 0;
  if (unrestricted_session_bus)
    g_debug ("Allowing session-dbus access");
  if (flatpak_run_add_session_dbus_args (argv_array, envp_p, session_bus_proxy_argv, unrestricted_session_bus) &&
      !unrestricted_session_bus && session_bus_proxy_argv)
    flatpak_add_bus_filters (session_bus_proxy_argv, context->session_bus_policy, app_id, context);

  unrestricted_system_bus = (context->sockets & FLATPAK_CONTEXT_SOCKET_SYSTEM_BUS) != 0;
  if (unrestricted_system_bus)
    g_debug ("Allowing system-dbus access");
  if (flatpak_run_add_system_dbus_args (context, envp_p, argv_array, system_bus_proxy_argv,
                                        unrestricted_system_bus) &&
      !unrestricted_system_bus && system_bus_proxy_argv)
    flatpak_add_bus_filters (system_bus_proxy_argv, context->system_bus_policy, NULL, context);

  if (g_environ_getenv (*envp_p, "LD_LIBRARY_PATH") != NULL)
    {
      /* LD_LIBRARY_PATH is overridden for setuid helper, so pass it as cmdline arg */
      add_args (argv_array,
                "--setenv", "LD_LIBRARY_PATH", g_environ_getenv (*envp_p, "LD_LIBRARY_PATH"),
                NULL);
      *envp_p = g_environ_unsetenv (*envp_p, "LD_LIBRARY_PATH");
    }
}