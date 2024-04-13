nfp_flower_non_repr_priv_put(struct nfp_app *app, struct net_device *netdev)
{
	struct nfp_flower_non_repr_priv *entry;

	entry = nfp_flower_non_repr_priv_lookup(app, netdev);
	if (!entry)
		return;

	__nfp_flower_non_repr_priv_put(entry);
}