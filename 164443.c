u32 nfp_flower_get_port_id_from_netdev(struct nfp_app *app,
				       struct net_device *netdev)
{
	int ext_port;

	if (nfp_netdev_is_nfp_repr(netdev)) {
		return nfp_repr_get_port_id(netdev);
	} else if (nfp_flower_internal_port_can_offload(app, netdev)) {
		ext_port = nfp_flower_get_internal_port_id(app, netdev);
		if (ext_port < 0)
			return 0;

		return nfp_flower_internal_port_get_port_id(ext_port);
	}

	return 0;
}