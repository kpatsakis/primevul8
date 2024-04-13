add_file_args (GPtrArray *argv_array,
               GHashTable *hash_table)
{
  guint n_keys;
  g_autofree const char **keys = (const char **)g_hash_table_get_keys_as_array (hash_table, &n_keys);
  guint i;

  g_qsort_with_data (keys, n_keys, sizeof (char *), (GCompareDataFunc) flatpak_strcmp0_ptr, NULL);

  for (i = 0; i < n_keys; i++)
    {
      const char *path = keys[i];
      guint mode;

      mode = GPOINTER_TO_INT (g_hash_table_lookup (hash_table, path));

      if (mode == FAKE_MODE_SYMLINK)
        {
          if (!path_is_visible (keys, n_keys, hash_table, path))
            {
              g_autofree char *resolved = flatpak_resolve_link (path, NULL);
              if (resolved)
                add_args (argv_array, "--symlink", resolved, path,  NULL);
            }
        }
      else if (mode == FAKE_MODE_HIDDEN)
        {
          /* Mount a tmpfs to hide the subdirectory, but only if
             either its not visible (then we can always create the
             dir on the tmpfs, or if there is a pre-existing dir
             we can mount the path on. */
          if (!path_is_visible (keys, n_keys, hash_table, path) ||
              g_file_test (path, G_FILE_TEST_IS_DIR))
            add_args (argv_array, "--tmpfs", path, NULL);
        }
      else
        add_args (argv_array,
                  (mode == FLATPAK_FILESYSTEM_MODE_READ_ONLY) ? "--ro-bind" : "--bind",
                  path, path, NULL);
    }
}