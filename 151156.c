flatpak_ensure_data_dir (const char   *app_id,
                         GCancellable *cancellable,
                         GError      **error)
{
  g_autoptr(GFile) dir = flatpak_get_data_dir (app_id);
  g_autoptr(GFile) data_dir = g_file_get_child (dir, "data");
  g_autoptr(GFile) cache_dir = g_file_get_child (dir, "cache");
  g_autoptr(GFile) tmp_dir = g_file_get_child (cache_dir, "tmp");
  g_autoptr(GFile) config_dir = g_file_get_child (dir, "config");

  if (!flatpak_mkdir_p (data_dir, cancellable, error))
    return NULL;

  if (!flatpak_mkdir_p (cache_dir, cancellable, error))
    return NULL;

  if (!flatpak_mkdir_p (tmp_dir, cancellable, error))
    return NULL;

  if (!flatpak_mkdir_p (config_dir, cancellable, error))
    return NULL;

  return g_object_ref (dir);
}