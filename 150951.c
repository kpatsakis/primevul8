ephy_embed_single_set_network_status (EphyEmbedSingle *single,
                                      gboolean status)
{
  if (status != single->priv->online)
    single->priv->online = status;
}