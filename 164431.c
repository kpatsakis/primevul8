nfp_flower_non_repr_priv_lookup(struct nfp_app *app, struct net_device *netdev)
{
	struct nfp_flower_priv *priv = app->priv;
	struct nfp_flower_non_repr_priv *entry;

	ASSERT_RTNL();

	list_for_each_entry(entry, &priv->non_repr_priv, list)
		if (entry->netdev == netdev)
			return entry;

	return NULL;
}