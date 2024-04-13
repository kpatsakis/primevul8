add_hide_path (GHashTable *hash_table,
               const char           *path)
{
  guint old_mode;

  old_mode = GPOINTER_TO_INT (g_hash_table_lookup (hash_table, path));
  g_hash_table_insert (hash_table, g_strdup (path),
                       GINT_TO_POINTER ( MAX (old_mode, FAKE_MODE_HIDDEN)));
}