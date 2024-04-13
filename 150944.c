ephy_embed_single_add_form_auth (EphyEmbedSingle *single,
                                 const char *uri,
                                 const char *form_username,
                                 const char *form_password,
                                 const char *username)
{
  EphyEmbedSingleFormAuthData *form_data;
  EphyEmbedSinglePrivate *priv;
  GSList *l;

  g_return_if_fail (EPHY_IS_EMBED_SINGLE (single));
  g_return_if_fail (uri);
  g_return_if_fail (form_username);
  g_return_if_fail (form_password);
  g_return_if_fail (username);

  priv = single->priv;

  LOG ("Appending: name field: %s / pass field: %s / username: %s / uri: %s", form_username, form_password, username, uri);

  form_data = form_auth_data_new (form_username, form_password, username);
  l = g_hash_table_lookup (priv->form_auth_data,
                           uri);
  l = g_slist_append (l, form_data);
  g_hash_table_replace (priv->form_auth_data,
                        g_strdup (uri),
                        l);
}