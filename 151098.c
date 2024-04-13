flatpak_run_add_app_info_args (GPtrArray      *argv_array,
                               GArray         *fd_array,
                               GFile          *app_files,
                               GFile          *runtime_files,
                               const char     *app_id,
                               const char     *app_branch,
                               const char     *runtime_ref,
                               FlatpakContext *final_app_context,
                               char          **app_info_path_out,
                               GError        **error)
{
  g_autofree char *tmp_path = NULL;
  int fd;
  g_autoptr(GKeyFile) keyfile = NULL;
  g_autofree char *runtime_path = NULL;
  g_autofree char *fd_str = NULL;
  g_autofree char *old_dest = g_strdup_printf ("/run/user/%d/flatpak-info", getuid ());
  const char *group;

  fd = g_file_open_tmp ("flatpak-context-XXXXXX", &tmp_path, NULL);
  if (fd < 0)
    {
      int errsv = errno;
      g_set_error (error, G_IO_ERROR, g_io_error_from_errno (errsv),
                   _("Failed to open flatpak-info temp file: %s"), g_strerror (errsv));
      return FALSE;
    }

  close (fd);

  keyfile = g_key_file_new ();

  if (app_files)
    group = "Application";
  else
    group = "Runtime";

  g_key_file_set_string (keyfile, group, "name", app_id);
  g_key_file_set_string (keyfile, group, "runtime", runtime_ref);

  if (app_files)
    {
      g_autofree char *app_path = g_file_get_path (app_files);
      g_key_file_set_string (keyfile, "Instance", "app-path", app_path);
    }
  runtime_path = g_file_get_path (runtime_files);
  g_key_file_set_string (keyfile, "Instance", "runtime-path", runtime_path);
  if (app_branch != NULL)
    g_key_file_set_string (keyfile, "Instance", "branch", app_branch);

  g_key_file_set_string (keyfile, "Instance", "flatpak-version", PACKAGE_VERSION);

  if ((final_app_context->sockets & FLATPAK_CONTEXT_SOCKET_SESSION_BUS) == 0)
    g_key_file_set_boolean (keyfile, "Instance", "session-bus-proxy", TRUE);

  if ((final_app_context->sockets & FLATPAK_CONTEXT_SOCKET_SYSTEM_BUS) == 0)
    g_key_file_set_boolean (keyfile, "Instance", "system-bus-proxy", TRUE);

  flatpak_context_save_metadata (final_app_context, TRUE, keyfile);

  if (!g_key_file_save_to_file (keyfile, tmp_path, error))
    return FALSE;

  fd = open (tmp_path, O_RDONLY);
  if (fd == -1)
    {
      int errsv = errno;
      g_set_error (error, G_IO_ERROR, g_io_error_from_errno (errsv),
                   _("Failed to open temp file: %s"), g_strerror (errsv));
      return FALSE;
    }

  unlink (tmp_path);

  fd_str = g_strdup_printf ("%d", fd);
  if (fd_array)
    g_array_append_val (fd_array, fd);

  add_args (argv_array,
            "--ro-bind-data", fd_str, "/.flatpak-info",
            "--symlink", "../../../.flatpak-info", old_dest,
            NULL);

  if (app_info_path_out != NULL)
    *app_info_path_out = g_strdup_printf ("/proc/self/fd/%d", fd);

  return TRUE;
}