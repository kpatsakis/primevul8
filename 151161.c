option_filesystem_cb (const gchar *option_name,
                      const gchar *value,
                      gpointer     data,
                      GError     **error)
{
  FlatpakContext *context = data;

  if (!flatpak_context_verify_filesystem (value, error))
    return FALSE;

  flatpak_context_add_filesystem (context, value);
  return TRUE;
}