flatpak_add_bus_filters (GPtrArray      *dbus_proxy_argv,
                         GHashTable     *ht,
                         const char     *app_id,
                         FlatpakContext *context)
{
  GHashTableIter iter;
  gpointer key, value;

  g_ptr_array_add (dbus_proxy_argv, g_strdup ("--filter"));
  if (app_id)
    {
      g_ptr_array_add (dbus_proxy_argv, g_strdup_printf ("--own=%s", app_id));
      g_ptr_array_add (dbus_proxy_argv, g_strdup_printf ("--own=%s.*", app_id));
    }

  g_hash_table_iter_init (&iter, ht);
  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      FlatpakPolicy policy = GPOINTER_TO_INT (value);

      if (policy > 0)
        g_ptr_array_add (dbus_proxy_argv, g_strdup_printf ("--%s=%s", flatpak_policy_to_string (policy), (char *) key));
    }
}