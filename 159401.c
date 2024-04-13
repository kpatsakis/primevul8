polkit_system_bus_name_get_property (GObject    *object,
                                     guint       prop_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
  PolkitSystemBusName *system_bus_name = POLKIT_SYSTEM_BUS_NAME (object);

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, system_bus_name->name);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}