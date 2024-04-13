polkit_system_bus_name_class_init (PolkitSystemBusNameClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->get_property = polkit_system_bus_name_get_property;
  gobject_class->set_property = polkit_system_bus_name_set_property;
  gobject_class->finalize     = polkit_system_bus_name_finalize;

  /**
   * PolkitSystemBusName:name:
   *
   * The unique name on the system message bus.
   */
  g_object_class_install_property (gobject_class,
                                   PROP_NAME,
                                   g_param_spec_string ("name",
                                                        "Name",
                                                        "The unique name on the system message bus",
                                                        NULL,
                                                        G_PARAM_CONSTRUCT |
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_BLURB |
                                                        G_PARAM_STATIC_NICK));

}