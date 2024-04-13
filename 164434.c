static void nfp_flower_internal_port_init(struct nfp_flower_priv *priv)
{
	spin_lock_init(&priv->internal_ports.lock);
	idr_init(&priv->internal_ports.port_ids);
}