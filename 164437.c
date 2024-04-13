static void nfp_flower_internal_port_cleanup(struct nfp_flower_priv *priv)
{
	idr_destroy(&priv->internal_ports.port_ids);
}