ephy_embed_single_get_property (GObject *object,
                                guint prop_id,
                                GValue *value,
                                GParamSpec *pspec)
{
  EphyEmbedSingle *single = EPHY_EMBED_SINGLE (object);

  switch (prop_id) {
  case PROP_NETWORK_STATUS:
    g_value_set_boolean (value, ephy_embed_single_get_network_status (single));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}