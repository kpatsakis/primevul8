nfp_flower_lookup_internal_port_id(struct nfp_flower_priv *priv,
				   struct net_device *netdev)
{
	struct net_device *entry;
	int i, id = 0;

	rcu_read_lock();
	idr_for_each_entry(&priv->internal_ports.port_ids, entry, i)
		if (entry == netdev) {
			id = i;
			break;
		}
	rcu_read_unlock();

	return id;
}