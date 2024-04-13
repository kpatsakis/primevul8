nfp_flower_repr_netdev_preclean(struct nfp_app *app, struct net_device *netdev)
{
	struct nfp_repr *repr = netdev_priv(netdev);
	struct nfp_flower_priv *priv = app->priv;
	atomic_t *replies = &priv->reify_replies;
	int err;

	atomic_set(replies, 0);
	err = nfp_flower_cmsg_portreify(repr, false);
	if (err) {
		nfp_warn(app->cpp, "Failed to notify firmware about repr destruction\n");
		return;
	}

	nfp_flower_wait_repr_reify(app, replies, 1);
}