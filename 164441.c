static int nfp_flower_vnic_init(struct nfp_app *app, struct nfp_net *nn)
{
	struct nfp_flower_priv *priv = app->priv;
	int err;

	priv->nn = nn;

	err = nfp_flower_spawn_phy_reprs(app, app->priv);
	if (err)
		goto err_clear_nn;

	err = nfp_flower_spawn_vnic_reprs(app,
					  NFP_FLOWER_CMSG_PORT_VNIC_TYPE_PF,
					  NFP_REPR_TYPE_PF, 1);
	if (err)
		goto err_destroy_reprs_phy;

	if (app->pf->num_vfs) {
		err = nfp_flower_spawn_vnic_reprs(app,
						  NFP_FLOWER_CMSG_PORT_VNIC_TYPE_VF,
						  NFP_REPR_TYPE_VF,
						  app->pf->num_vfs);
		if (err)
			goto err_destroy_reprs_pf;
	}

	return 0;

err_destroy_reprs_pf:
	nfp_reprs_clean_and_free_by_type(app, NFP_REPR_TYPE_PF);
err_destroy_reprs_phy:
	nfp_reprs_clean_and_free_by_type(app, NFP_REPR_TYPE_PHYS_PORT);
err_clear_nn:
	priv->nn = NULL;
	return err;
}