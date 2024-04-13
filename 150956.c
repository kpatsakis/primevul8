ephy_embed_single_finalize (GObject *object)
{
  EphyEmbedSinglePrivate *priv = EPHY_EMBED_SINGLE (object)->priv;

  ephy_embed_prefs_shutdown ();

  if (priv->form_auth_data) {
    g_hash_table_foreach (priv->form_auth_data,
                          (GHFunc)remove_form_auth_data,
                          NULL);
    g_hash_table_destroy (priv->form_auth_data);
  }

  G_OBJECT_CLASS (ephy_embed_single_parent_class)->finalize (object);
}