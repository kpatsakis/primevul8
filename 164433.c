static void nfp_flower_sriov_disable(struct nfp_app *app)
{
	struct nfp_flower_priv *priv = app->priv;

	if (!priv->nn)
		return;

	nfp_reprs_clean_and_free_by_type(app, NFP_REPR_TYPE_VF);
}