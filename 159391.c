polkit_system_bus_name_set_name (PolkitSystemBusName *system_bus_name,
                                 const gchar         *name)
{
  g_return_if_fail (POLKIT_IS_SYSTEM_BUS_NAME (system_bus_name));
  g_return_if_fail (g_dbus_is_unique_name (name));
  g_free (system_bus_name->name);
  system_bus_name->name = g_strdup (name);
}