flatpak_run_app (const char     *app_ref,
                 FlatpakDeploy  *app_deploy,
                 FlatpakContext *extra_context,
                 const char     *custom_runtime,
                 const char     *custom_runtime_version,
                 FlatpakRunFlags flags,
                 const char     *custom_command,
                 char           *args[],
                 int             n_args,
                 GCancellable   *cancellable,
                 GError        **error)
{
  g_autoptr(FlatpakDeploy) runtime_deploy = NULL;
  g_autoptr(GFile) app_files = NULL;
  g_autoptr(GFile) runtime_files = NULL;
  g_autoptr(GFile) app_id_dir = NULL;
  g_autofree char *default_runtime = NULL;
  g_autofree char *default_command = NULL;
  g_autofree char *runtime_ref = NULL;
  int sync_fds[2] = {-1, -1};
  g_autoptr(GKeyFile) metakey = NULL;
  g_autoptr(GKeyFile) runtime_metakey = NULL;
  g_autoptr(GPtrArray) argv_array = NULL;
  g_autoptr(GArray) fd_array = NULL;
  g_autoptr(GPtrArray) real_argv_array = NULL;
  g_auto(GStrv) envp = NULL;
  g_autoptr(GPtrArray) session_bus_proxy_argv = NULL;
  g_autoptr(GPtrArray) system_bus_proxy_argv = NULL;
  const char *command = "/bin/sh";
  g_autoptr(GError) my_error = NULL;
  g_auto(GStrv) runtime_parts = NULL;
  int i;
  g_autofree char *app_info_path = NULL;
  g_autoptr(FlatpakContext) app_context = NULL;
  g_autoptr(FlatpakContext) overrides = NULL;
  g_auto(GStrv) app_ref_parts = NULL;

  app_ref_parts = flatpak_decompose_ref (app_ref, error);
  if (app_ref_parts == NULL)
    return FALSE;

  argv_array = g_ptr_array_new_with_free_func (g_free);
  fd_array = g_array_new (FALSE, TRUE, sizeof (int));
  g_array_set_clear_func (fd_array, clear_fd);

  session_bus_proxy_argv = g_ptr_array_new_with_free_func (g_free);
  system_bus_proxy_argv = g_ptr_array_new_with_free_func (g_free);

  if (app_deploy == NULL)
    {
      g_assert (g_str_has_prefix (app_ref, "runtime/"));
      default_runtime = g_strdup (app_ref + strlen ("runtime/"));
    }
  else
    {
      metakey = flatpak_deploy_get_metadata (app_deploy);
      default_runtime = g_key_file_get_string (metakey, "Application",
                                               (flags & FLATPAK_RUN_FLAG_DEVEL) != 0 ? "sdk" : "runtime",
                                               &my_error);
      if (my_error)
        {
          g_propagate_error (error, g_steal_pointer (&my_error));
          return FALSE;
        }
    }

  runtime_parts = g_strsplit (default_runtime, "/", 0);
  if (g_strv_length (runtime_parts) != 3)
    return flatpak_fail (error, "Wrong number of components in runtime %s", default_runtime);

  if (custom_runtime)
    {
      g_auto(GStrv) custom_runtime_parts = g_strsplit (custom_runtime, "/", 0);

      for (i = 0; i < 3 && custom_runtime_parts[i] != NULL; i++)
        {
          if (strlen (custom_runtime_parts[i]) > 0)
            {
              g_free (runtime_parts[i]);
              runtime_parts[i] = g_steal_pointer (&custom_runtime_parts[i]);
            }
        }
    }

  if (custom_runtime_version)
    {
      g_free (runtime_parts[2]);
      runtime_parts[2] = g_strdup (custom_runtime_version);
    }

  runtime_ref = flatpak_compose_ref (FALSE,
                                     runtime_parts[0],
                                     runtime_parts[2],
                                     runtime_parts[1],
                                     error);
  if (runtime_ref == NULL)
    return FALSE;

  runtime_deploy = flatpak_find_deploy_for_ref (runtime_ref, cancellable, error);
  if (runtime_deploy == NULL)
    return FALSE;

  runtime_metakey = flatpak_deploy_get_metadata (runtime_deploy);

  app_context = compute_permissions (metakey, runtime_metakey, error);
  if (app_context == NULL)
    return FALSE;

  if (app_deploy != NULL)
    {
      overrides = flatpak_deploy_get_overrides (app_deploy);
      flatpak_context_merge (app_context, overrides);
    }

  if (extra_context)
    flatpak_context_merge (app_context, extra_context);

  runtime_files = flatpak_deploy_get_files (runtime_deploy);
  if (app_deploy != NULL)
    {
      app_files = flatpak_deploy_get_files (app_deploy);
      if ((app_id_dir = flatpak_ensure_data_dir (app_ref_parts[1], cancellable, error)) == NULL)
        return FALSE;
    }

  envp = g_get_environ ();
  envp = flatpak_run_apply_env_default (envp);
  envp = flatpak_run_apply_env_vars (envp, app_context);
  if (app_id_dir != NULL)
    envp = flatpak_run_apply_env_appid (envp, app_id_dir);

  add_args (argv_array,
            "--ro-bind", flatpak_file_get_path_cached (runtime_files), "/usr",
            "--lock-file", "/usr/.ref",
            NULL);

  if (app_files != NULL)
    add_args (argv_array,
              "--ro-bind", flatpak_file_get_path_cached (app_files), "/app",
              "--lock-file", "/app/.ref",
              NULL);
  else
    add_args (argv_array,
              "--dir", "/app",
              NULL);

  if (app_context->features & FLATPAK_CONTEXT_FEATURE_DEVEL)
    flags |= FLATPAK_RUN_FLAG_DEVEL;

  if (app_context->features & FLATPAK_CONTEXT_FEATURE_MULTIARCH)
    flags |= FLATPAK_RUN_FLAG_MULTIARCH;

  if (!flatpak_run_setup_base_argv (argv_array, fd_array, runtime_files, app_id_dir, app_ref_parts[2], flags, error))
    return FALSE;

  if (!flatpak_run_add_app_info_args (argv_array, fd_array, app_files, runtime_files, app_ref_parts[1], app_ref_parts[3],
                                      runtime_ref, app_context, &app_info_path, error))
    return FALSE;

  if (metakey != NULL &&
      !flatpak_run_add_extension_args (argv_array, metakey, app_ref, cancellable, error))
    return FALSE;

  if (!flatpak_run_add_extension_args (argv_array, runtime_metakey, runtime_ref, cancellable, error))
    return FALSE;

  add_document_portal_args (argv_array, app_ref_parts[1]);

  flatpak_run_add_environment_args (argv_array, fd_array, &envp,
                                    session_bus_proxy_argv,
                                    system_bus_proxy_argv,
                                    app_ref_parts[1], app_context, app_id_dir);
  flatpak_run_add_journal_args (argv_array);
  add_font_path_args (argv_array);

  /* Must run this before spawning the dbus proxy, to ensure it
     ends up in the app cgroup */
  if (!flatpak_run_in_transient_unit (app_ref_parts[1], &my_error))
    {
      /* We still run along even if we don't get a cgroup, as nothing
         really depends on it. Its just nice to have */
      g_debug ("Failed to run in transient scope: %s\n", my_error->message);
      g_clear_error (&my_error);
    }

  if (!add_dbus_proxy_args (argv_array, session_bus_proxy_argv, (flags & FLATPAK_RUN_FLAG_LOG_SESSION_BUS) != 0,
                            sync_fds, app_info_path, error))
    return FALSE;

  if (!add_dbus_proxy_args (argv_array, system_bus_proxy_argv, (flags & FLATPAK_RUN_FLAG_LOG_SYSTEM_BUS) != 0,
                            sync_fds, app_info_path, error))
    return FALSE;

  if (sync_fds[1] != -1)
    close (sync_fds[1]);

  add_args (argv_array,
            /* Not in base, because we don't want this for flatpak build */
            "--symlink", "/app/lib/debug/source", "/run/build",
            "--symlink", "/usr/lib/debug/source", "/run/build-runtime",
            NULL);

  if (custom_command)
    {
      command = custom_command;
    }
  else if (metakey)
    {
      default_command = g_key_file_get_string (metakey, "Application", "command", &my_error);
      if (my_error)
        {
          g_propagate_error (error, g_steal_pointer (&my_error));
          return FALSE;
        }
      command = default_command;
    }

  real_argv_array = g_ptr_array_new_with_free_func (g_free);
  g_ptr_array_add (real_argv_array, g_strdup (flatpak_get_bwrap ()));

  {
    gsize len;
    int arg_fd;
    g_autofree char *arg_fd_str = NULL;
    g_autofree char *args = join_args (argv_array, &len);

    arg_fd = create_tmp_fd (args, len, error);
    if (arg_fd < 0)
      return FALSE;

    arg_fd_str = g_strdup_printf ("%d", arg_fd);
    g_array_append_val (fd_array, arg_fd);

    add_args (real_argv_array,
              "--args", arg_fd_str,
              NULL);
  }

  g_ptr_array_add (real_argv_array, g_strdup (command));
  for (i = 0; i < n_args; i++)
    g_ptr_array_add (real_argv_array, g_strdup (args[i]));

  g_ptr_array_add (real_argv_array, NULL);

  if ((flags & FLATPAK_RUN_FLAG_BACKGROUND) != 0)
    {
      if (!g_spawn_async (NULL,
                          (char **) real_argv_array->pdata,
                          envp,
                          G_SPAWN_SEARCH_PATH,
                          child_setup, fd_array,
                          NULL,
                          error))
        return FALSE;
    }
  else
    {
      if (execvpe (flatpak_get_bwrap (), (char **) real_argv_array->pdata, envp) == -1)
        {
          g_set_error_literal (error, G_IO_ERROR, g_io_error_from_errno (errno),
                               _("Unable to start app"));
          return FALSE;
        }
      /* Not actually reached... */
    }

  return TRUE;
}