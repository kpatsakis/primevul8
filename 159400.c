polkit_system_bus_name_finalize (GObject *object)
{
  PolkitSystemBusName *system_bus_name = POLKIT_SYSTEM_BUS_NAME (object);

  g_free (system_bus_name->name);

  if (G_OBJECT_CLASS (polkit_system_bus_name_parent_class)->finalize != NULL)
    G_OBJECT_CLASS (polkit_system_bus_name_parent_class)->finalize (object);
}