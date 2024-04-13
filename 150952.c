ephy_embed_single_get_form_auth (EphyEmbedSingle *single,
                                 const char *uri)
{
  EphyEmbedSinglePrivate *priv;

  g_return_val_if_fail (EPHY_IS_EMBED_SINGLE (single), NULL);
  g_return_val_if_fail (uri, NULL);

  priv = single->priv;

  return g_hash_table_lookup (priv->form_auth_data, uri);
}