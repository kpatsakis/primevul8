static void handle_rx_net(struct vhost_work *work)
{
	struct vhost_net *net = container_of(work, struct vhost_net,
					     poll[VHOST_NET_VQ_RX].work);
	handle_rx(net);
}