nfp_flower_free_internal_port_id(struct nfp_app *app, struct net_device *netdev)
{
	struct nfp_flower_priv *priv = app->priv;
	int id;

	id = nfp_flower_lookup_internal_port_id(priv, netdev);
	if (!id)
		return;

	spin_lock_bh(&priv->internal_ports.lock);
	idr_remove(&priv->internal_ports.port_ids, id);
	spin_unlock_bh(&priv->internal_ports.lock);
}