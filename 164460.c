nfp_flower_dev_get(struct nfp_app *app, u32 port_id, bool *redir_egress)
{
	enum nfp_repr_type repr_type;
	struct nfp_reprs *reprs;
	u8 port = 0;

	/* Check if the port is internal. */
	if (FIELD_GET(NFP_FLOWER_CMSG_PORT_TYPE, port_id) ==
	    NFP_FLOWER_CMSG_PORT_TYPE_OTHER_PORT) {
		if (redir_egress)
			*redir_egress = true;
		port = FIELD_GET(NFP_FLOWER_CMSG_PORT_PHYS_PORT_NUM, port_id);
		return nfp_flower_get_netdev_from_internal_port_id(app, port);
	}

	repr_type = nfp_flower_repr_get_type_and_port(app, port_id, &port);
	if (repr_type > NFP_REPR_TYPE_MAX)
		return NULL;

	reprs = rcu_dereference(app->reprs[repr_type]);
	if (!reprs)
		return NULL;

	if (port >= reprs->num_reprs)
		return NULL;

	return rcu_dereference(reprs->reprs[port]);
}