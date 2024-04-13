nfp_flower_netdev_event(struct nfp_app *app, struct net_device *netdev,
			unsigned long event, void *ptr)
{
	struct nfp_flower_priv *app_priv = app->priv;
	int ret;

	if (app_priv->flower_ext_feats & NFP_FL_FEATS_LAG) {
		ret = nfp_flower_lag_netdev_event(app_priv, netdev, event, ptr);
		if (ret & NOTIFY_STOP_MASK)
			return ret;
	}

	ret = nfp_flower_reg_indir_block_handler(app, netdev, event);
	if (ret & NOTIFY_STOP_MASK)
		return ret;

	ret = nfp_flower_internal_port_event_handler(app, netdev, event);
	if (ret & NOTIFY_STOP_MASK)
		return ret;

	return nfp_tunnel_mac_event_handler(app, netdev, event, ptr);
}