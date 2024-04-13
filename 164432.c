nfp_flower_non_repr_priv_get(struct nfp_app *app, struct net_device *netdev)
{
	struct nfp_flower_priv *priv = app->priv;
	struct nfp_flower_non_repr_priv *entry;

	entry = nfp_flower_non_repr_priv_lookup(app, netdev);
	if (entry)
		goto inc_ref;

	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return NULL;

	entry->netdev = netdev;
	list_add(&entry->list, &priv->non_repr_priv);

inc_ref:
	__nfp_flower_non_repr_priv_get(entry);
	return entry;
}