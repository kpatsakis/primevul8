nfp_flower_get_internal_port_id(struct nfp_app *app, struct net_device *netdev)
{
	struct nfp_flower_priv *priv = app->priv;
	int id;

	id = nfp_flower_lookup_internal_port_id(priv, netdev);
	if (id > 0)
		return id;

	idr_preload(GFP_ATOMIC);
	spin_lock_bh(&priv->internal_ports.lock);
	id = idr_alloc(&priv->internal_ports.port_ids, netdev,
		       NFP_MIN_INT_PORT_ID, NFP_MAX_INT_PORT_ID, GFP_ATOMIC);
	spin_unlock_bh(&priv->internal_ports.lock);
	idr_preload_end();

	return id;
}