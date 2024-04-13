static int nfp_flower_start(struct nfp_app *app)
{
	struct nfp_flower_priv *app_priv = app->priv;
	int err;

	if (app_priv->flower_ext_feats & NFP_FL_FEATS_LAG) {
		err = nfp_flower_lag_reset(&app_priv->nfp_lag);
		if (err)
			return err;
	}

	return nfp_tunnel_config_start(app);
}