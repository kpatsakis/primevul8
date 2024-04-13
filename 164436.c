static bool nfp_flower_check_ack(struct nfp_flower_priv *app_priv)
{
	bool ret;

	spin_lock_bh(&app_priv->mtu_conf.lock);
	ret = app_priv->mtu_conf.ack;
	spin_unlock_bh(&app_priv->mtu_conf.lock);

	return ret;
}