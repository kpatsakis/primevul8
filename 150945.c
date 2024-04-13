ephy_embed_single_set_property (GObject *object,
                                guint prop_id,
                                const GValue *value,
                                GParamSpec *pspec)
{
  EphyEmbedSingle *single = EPHY_EMBED_SINGLE (object);

  switch (prop_id) {
  case PROP_NETWORK_STATUS:
    ephy_embed_single_set_network_status (single, g_value_get_boolean (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}