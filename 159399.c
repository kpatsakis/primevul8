polkit_system_bus_name_new (const gchar *name)
{
  g_return_val_if_fail (g_dbus_is_unique_name (name), NULL);
  return POLKIT_SUBJECT (g_object_new (POLKIT_TYPE_SYSTEM_BUS_NAME,
                                       "name", name,
                                       NULL));
}