nfp_flower_repr_change_mtu(struct nfp_app *app, struct net_device *netdev,
			   int new_mtu)
{
	struct nfp_flower_priv *app_priv = app->priv;
	struct nfp_repr *repr = netdev_priv(netdev);
	int err;

	/* Only need to config FW for physical port MTU change. */
	if (repr->port->type != NFP_PORT_PHYS_PORT)
		return 0;

	if (!(app_priv->flower_ext_feats & NFP_FL_NBI_MTU_SETTING)) {
		nfp_err(app->cpp, "Physical port MTU setting not supported\n");
		return -EINVAL;
	}

	spin_lock_bh(&app_priv->mtu_conf.lock);
	app_priv->mtu_conf.ack = false;
	app_priv->mtu_conf.requested_val = new_mtu;
	app_priv->mtu_conf.portnum = repr->dst->u.port_info.port_id;
	spin_unlock_bh(&app_priv->mtu_conf.lock);

	err = nfp_flower_cmsg_portmod(repr, netif_carrier_ok(netdev), new_mtu,
				      true);
	if (err) {
		spin_lock_bh(&app_priv->mtu_conf.lock);
		app_priv->mtu_conf.requested_val = 0;
		spin_unlock_bh(&app_priv->mtu_conf.lock);
		return err;
	}

	/* Wait for fw to ack the change. */
	if (!wait_event_timeout(app_priv->mtu_conf.wait_q,
				nfp_flower_check_ack(app_priv),
				NFP_FL_REPLY_TIMEOUT)) {
		spin_lock_bh(&app_priv->mtu_conf.lock);
		app_priv->mtu_conf.requested_val = 0;
		spin_unlock_bh(&app_priv->mtu_conf.lock);
		nfp_warn(app->cpp, "MTU change not verified with fw\n");
		return -EIO;
	}

	return 0;
}