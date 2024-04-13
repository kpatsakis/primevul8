polkit_system_bus_name_to_string (PolkitSubject *subject)
{
  PolkitSystemBusName *system_bus_name = POLKIT_SYSTEM_BUS_NAME (subject);

  return g_strdup_printf ("system-bus-name:%s", system_bus_name->name);
}