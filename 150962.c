ephy_embed_single_init (EphyEmbedSingle *single)
{
  EphyEmbedSinglePrivate *priv;

  single->priv = priv = EPHY_EMBED_SINGLE_GET_PRIVATE (single);
  priv->online = TRUE;

  priv->form_auth_data = g_hash_table_new_full (g_str_hash,
                                                g_str_equal,
                                                g_free,
                                                NULL);
  cache_keyring_form_data (single);
}