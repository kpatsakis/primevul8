nfp_flower_internal_port_event_handler(struct nfp_app *app,
				       struct net_device *netdev,
				       unsigned long event)
{
	if (event == NETDEV_UNREGISTER &&
	    nfp_flower_internal_port_can_offload(app, netdev))
		nfp_flower_free_internal_port_id(app, netdev);

	return NOTIFY_OK;
}