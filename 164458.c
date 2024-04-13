static void nfp_flower_vnic_clean(struct nfp_app *app, struct nfp_net *nn)
{
	struct nfp_flower_priv *priv = app->priv;

	if (app->pf->num_vfs)
		nfp_reprs_clean_and_free_by_type(app, NFP_REPR_TYPE_VF);
	nfp_reprs_clean_and_free_by_type(app, NFP_REPR_TYPE_PF);
	nfp_reprs_clean_and_free_by_type(app, NFP_REPR_TYPE_PHYS_PORT);

	priv->nn = NULL;
}