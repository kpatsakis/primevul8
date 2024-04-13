flatpak_context_new (void)
{
  FlatpakContext *context;

  context = g_slice_new0 (FlatpakContext);
  context->env_vars = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
  context->persistent = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  context->filesystems = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  context->session_bus_policy = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  context->system_bus_policy = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  context->generic_policy = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                   g_free, (GDestroyNotify)g_strfreev);

  return context;
}