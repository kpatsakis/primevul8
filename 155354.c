static void vhost_net_tx_packet(struct vhost_net *net)
{
	++net->tx_packets;
	if (net->tx_packets < 1024)
		return;
	net->tx_packets = 0;
	net->tx_zcopy_err = 0;
}