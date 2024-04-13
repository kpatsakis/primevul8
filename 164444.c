static int nfp_flower_sriov_enable(struct nfp_app *app, int num_vfs)
{
	struct nfp_flower_priv *priv = app->priv;

	if (!priv->nn)
		return 0;

	return nfp_flower_spawn_vnic_reprs(app,
					   NFP_FLOWER_CMSG_PORT_VNIC_TYPE_VF,
					   NFP_REPR_TYPE_VF, num_vfs);
}