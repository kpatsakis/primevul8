polkit_system_bus_name_set_property (GObject      *object,
                                     guint         prop_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  PolkitSystemBusName *system_bus_name = POLKIT_SYSTEM_BUS_NAME (object);

  switch (prop_id)
    {
    case PROP_NAME:
      polkit_system_bus_name_set_name (system_bus_name, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}