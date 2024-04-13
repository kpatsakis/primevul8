flatpak_run_setup_base_argv (GPtrArray      *argv_array,
                             GArray         *fd_array,
                             GFile          *runtime_files,
                             GFile          *app_id_dir,
                             const char     *arch,
                             FlatpakRunFlags flags,
                             GError        **error)
{
  const char *usr_links[] = {"lib", "lib32", "lib64", "bin", "sbin"};
  g_autofree char *run_dir = g_strdup_printf ("/run/user/%d", getuid ());
  int i;
  int passwd_fd = -1;
  g_autofree char *passwd_fd_str = NULL;
  g_autofree char *passwd_contents = NULL;
  int group_fd = -1;
  g_autofree char *group_fd_str = NULL;
  g_autofree char *group_contents = NULL;
  struct group *g = getgrgid (getgid ());

  g_autoptr(GFile) etc = NULL;

  passwd_contents = g_strdup_printf ("%s:x:%d:%d:%s:%s:%s\n"
                                     "nfsnobody:x:65534:65534:Unmapped user:/:/sbin/nologin\n",
                                     g_get_user_name (),
                                     getuid (), getgid (),
                                     g_get_real_name (),
                                     g_get_home_dir (),
                                     DEFAULT_SHELL);

  if ((passwd_fd = create_tmp_fd (passwd_contents, -1, error)) < 0)
    return FALSE;
  passwd_fd_str = g_strdup_printf ("%d", passwd_fd);
  if (fd_array)
    g_array_append_val (fd_array, passwd_fd);

  group_contents = g_strdup_printf ("%s:x:%d:%s\n"
                                    "nfsnobody:x:65534:\n",
                                    g->gr_name,
                                    getgid (), g_get_user_name ());
  if ((group_fd = create_tmp_fd (group_contents, -1, error)) < 0)
    return FALSE;
  group_fd_str = g_strdup_printf ("%d", group_fd);
  if (fd_array)
    g_array_append_val (fd_array, group_fd);

  add_args (argv_array,
            "--unshare-pid",
            "--unshare-user-try",
            "--proc", "/proc",
            "--dir", "/tmp",
            "--dir", "/var/tmp",
            "--dir", "/run/host",
            "--dir", run_dir,
            "--setenv", "XDG_RUNTIME_DIR", run_dir,
            "--symlink", "../run", "/var/run",
            "--ro-bind", "/sys/block", "/sys/block",
            "--ro-bind", "/sys/bus", "/sys/bus",
            "--ro-bind", "/sys/class", "/sys/class",
            "--ro-bind", "/sys/dev", "/sys/dev",
            "--ro-bind", "/sys/devices", "/sys/devices",
            "--bind-data", passwd_fd_str, "/etc/passwd",
            "--bind-data", group_fd_str, "/etc/group",
            /* Always create a homedir to start from, although it may be covered later */
            "--dir", g_get_home_dir (),
            NULL);

  if (g_file_test ("/etc/machine-id", G_FILE_TEST_EXISTS))
    add_args (argv_array, "--bind", "/etc/machine-id", "/etc/machine-id", NULL);
  else if (g_file_test ("/var/lib/dbus/machine-id", G_FILE_TEST_EXISTS))
    add_args (argv_array, "--bind", "/var/lib/dbus/machine-id", "/etc/machine-id", NULL);

  etc = g_file_get_child (runtime_files, "etc");
  if (g_file_query_exists (etc, NULL))
    {
      g_auto(GLnxDirFdIterator) dfd_iter = { 0, };
      struct dirent *dent;
      char path_buffer[PATH_MAX + 1];
      ssize_t symlink_size;

      glnx_dirfd_iterator_init_at (AT_FDCWD, flatpak_file_get_path_cached (etc), FALSE, &dfd_iter, NULL);

      while (TRUE)
        {
          g_autofree char *src = NULL;
          g_autofree char *dest = NULL;

          if (!glnx_dirfd_iterator_next_dent_ensure_dtype (&dfd_iter, &dent, NULL, NULL) || dent == NULL)
            break;

          if (strcmp (dent->d_name, "passwd") == 0 ||
              strcmp (dent->d_name, "group") == 0 ||
              strcmp (dent->d_name, "machine-id") == 0 ||
              strcmp (dent->d_name, "resolv.conf") == 0 ||
              strcmp (dent->d_name, "localtime") == 0)
            continue;

          src = g_build_filename (flatpak_file_get_path_cached (etc), dent->d_name, NULL);
          dest = g_build_filename ("/etc", dent->d_name, NULL);
          if (dent->d_type == DT_LNK)
            {
              symlink_size = readlinkat (dfd_iter.fd, dent->d_name, path_buffer, sizeof (path_buffer) - 1);
              if (symlink_size < 0)
                {
                  glnx_set_error_from_errno (error);
                  return FALSE;
                }
              path_buffer[symlink_size] = 0;
              add_args (argv_array, "--symlink", path_buffer, dest, NULL);
            }
          else
            {
              add_args (argv_array, "--bind", src, dest, NULL);
            }
        }
    }

  if (app_id_dir != NULL)
    {
      g_autoptr(GFile) app_cache_dir = g_file_get_child (app_id_dir, "cache");
      g_autoptr(GFile) app_tmp_dir = g_file_get_child (app_cache_dir, "tmp");
      g_autoptr(GFile) app_data_dir = g_file_get_child (app_id_dir, "data");
      g_autoptr(GFile) app_config_dir = g_file_get_child (app_id_dir, "config");

      add_args (argv_array,
                /* These are nice to have as a fixed path */
                "--bind", flatpak_file_get_path_cached (app_cache_dir), "/var/cache",
                "--bind", flatpak_file_get_path_cached (app_data_dir), "/var/data",
                "--bind", flatpak_file_get_path_cached (app_config_dir), "/var/config",
                "--bind", flatpak_file_get_path_cached (app_tmp_dir), "/var/tmp",
                NULL);
    }

  for (i = 0; i < G_N_ELEMENTS (usr_links); i++)
    {
      const char *subdir = usr_links[i];
      g_autoptr(GFile) runtime_subdir = g_file_get_child (runtime_files, subdir);
      if (g_file_query_exists (runtime_subdir, NULL))
        {
          g_autofree char *link = g_strconcat ("usr/", subdir, NULL);
          g_autofree char *dest = g_strconcat ("/", subdir, NULL);
          add_args (argv_array,
                    "--symlink", link, dest,
                    NULL);
        }
    }


#ifdef ENABLE_SECCOMP
  if (!setup_seccomp (argv_array,
                      fd_array,
                      arch,
                      (flags & FLATPAK_RUN_FLAG_MULTIARCH) != 0,
                      (flags & FLATPAK_RUN_FLAG_DEVEL) != 0,
                      error))
    return FALSE;
#endif

  add_monitor_path_args ((flags & FLATPAK_RUN_FLAG_NO_SESSION_HELPER) == 0, argv_array);

  return TRUE;
}