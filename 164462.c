static void nfp_flower_clean(struct nfp_app *app)
{
	struct nfp_flower_priv *app_priv = app->priv;

	skb_queue_purge(&app_priv->cmsg_skbs_high);
	skb_queue_purge(&app_priv->cmsg_skbs_low);
	flush_work(&app_priv->cmsg_work);

	if (app_priv->flower_ext_feats & NFP_FL_FEATS_VF_RLIM)
		nfp_flower_qos_cleanup(app);

	if (app_priv->flower_ext_feats & NFP_FL_FEATS_LAG)
		nfp_flower_lag_cleanup(&app_priv->nfp_lag);

	if (app_priv->flower_ext_feats & NFP_FL_FEATS_FLOW_MERGE)
		nfp_flower_internal_port_cleanup(app_priv);

	nfp_flower_metadata_cleanup(app);
	vfree(app->priv);
	app->priv = NULL;
}