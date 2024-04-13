static int nfp_flower_vnic_alloc(struct nfp_app *app, struct nfp_net *nn,
				 unsigned int id)
{
	if (id > 0) {
		nfp_warn(app->cpp, "FlowerNIC doesn't support more than one data vNIC\n");
		goto err_invalid_port;
	}

	eth_hw_addr_random(nn->dp.netdev);
	netif_keep_dst(nn->dp.netdev);
	nn->vnic_no_name = true;

	return 0;

err_invalid_port:
	nn->port = nfp_port_alloc(app, NFP_PORT_INVALID, nn->dp.netdev);
	return PTR_ERR_OR_ZERO(nn->port);
}