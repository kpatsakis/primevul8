flatpak_run_add_extension_args (GPtrArray    *argv_array,
                                GKeyFile     *metakey,
                                const char   *full_ref,
                                GCancellable *cancellable,
                                GError      **error)
{
  g_auto(GStrv) parts = NULL;
  gboolean is_app;
  GList *extensions, *l;

  parts = g_strsplit (full_ref, "/", 0);
  if (g_strv_length (parts) != 4)
    return flatpak_fail (error, "Failed to determine parts from ref: %s", full_ref);

  is_app = strcmp (parts[0], "app") == 0;

  extensions = flatpak_list_extensions (metakey,
                                        parts[2], parts[3]);

  for (l = extensions; l != NULL; l = l->next)
    {
      FlatpakExtension *ext = l->data;
      g_autofree char *full_directory = g_build_filename (is_app ? "/app" : "/usr", ext->directory, NULL);
      g_autofree char *ref = g_build_filename (full_directory, ".ref", NULL);
      g_autofree char *real_ref = g_build_filename (ext->files_path, ext->directory, ".ref", NULL);

      if (ext->needs_tmpfs)
        {
          g_autofree char *parent = g_path_get_dirname (full_directory);
          add_args (argv_array,
                    "--tmpfs", parent,
                    NULL);
        }

      add_args (argv_array,
                "--bind", ext->files_path, full_directory,
                NULL);

      if (g_file_test (real_ref, G_FILE_TEST_EXISTS))
        add_args (argv_array,
                  "--lock-file", ref,
                  NULL);
    }

  g_list_free_full (extensions, (GDestroyNotify) flatpak_extension_free);

  return TRUE;
}