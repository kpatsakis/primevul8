polkit_system_bus_name_get_name (PolkitSystemBusName *system_bus_name)
{
  g_return_val_if_fail (POLKIT_IS_SYSTEM_BUS_NAME (system_bus_name), NULL);
  return system_bus_name->name;
}