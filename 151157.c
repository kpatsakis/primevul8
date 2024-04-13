path_is_visible (const char **keys,
                 guint n_keys,
                 GHashTable *hash_table,
                 const char *path)
{
  guint i;
  gboolean is_visible = FALSE;

  /* The keys are sorted so shorter (i.e. parents) are first */
  for (i = 0; i < n_keys; i++)
    {
      const char *mounted_path = keys[i];
      guint mode;
      mode = GPOINTER_TO_INT (g_hash_table_lookup (hash_table, mounted_path));

      if (flatpak_has_path_prefix (path, mounted_path))
        {
          if (mode == FAKE_MODE_HIDDEN)
            is_visible = FALSE;
          else if (mode != FAKE_MODE_SYMLINK)
            is_visible = TRUE;
        }
    }

  return is_visible;
}