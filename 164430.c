nfp_flower_get_netdev_from_internal_port_id(struct nfp_app *app, int port_id)
{
	struct nfp_flower_priv *priv = app->priv;
	struct net_device *netdev;

	rcu_read_lock();
	netdev = idr_find(&priv->internal_ports.port_ids, port_id);
	rcu_read_unlock();

	return netdev;
}