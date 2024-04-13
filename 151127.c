add_expose_path (GHashTable *hash_table,
                 FlatpakFilesystemMode mode,
                 const char           *path)
{
  struct stat st;

  if (lstat (path, &st) != 0)
    return;

  if (S_ISDIR (st.st_mode) ||
      S_ISREG (st.st_mode) ||
      S_ISLNK (st.st_mode) ||
      S_ISSOCK (st.st_mode))
    {
      guint old_mode;

      old_mode = GPOINTER_TO_INT (g_hash_table_lookup (hash_table, path));

      if (S_ISLNK (st.st_mode))
        {
          g_autofree char *resolved = flatpak_resolve_link (path, NULL);
          /* Don't keep symlinks into /app or /usr, as they are not the
             same as on the host, and we generally can't create the parents
             for them anyway */
          if (resolved &&
              !g_str_has_prefix (resolved, "/app/")  &&
              !g_str_has_prefix (resolved, "/usr/"))
            {
              add_expose_path (hash_table, mode, resolved);
              mode = FAKE_MODE_SYMLINK;
            }
          else
            mode = 0;
        }
      if (mode > 0)
        g_hash_table_insert (hash_table, g_strdup (path), GINT_TO_POINTER ( MAX (old_mode, mode)));
    }
}