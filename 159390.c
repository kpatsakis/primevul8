polkit_system_bus_name_hash (PolkitSubject *subject)
{
  PolkitSystemBusName *system_bus_name = POLKIT_SYSTEM_BUS_NAME (subject);

  return g_str_hash (system_bus_name->name);
}