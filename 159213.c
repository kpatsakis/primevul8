static int vhost_net_open(struct inode *inode, struct file *f)
{
	struct vhost_net *n;
	struct vhost_dev *dev;
	struct vhost_virtqueue **vqs;
	void **queue;
	int i;

	n = kvmalloc(sizeof *n, GFP_KERNEL | __GFP_RETRY_MAYFAIL);
	if (!n)
		return -ENOMEM;
	vqs = kmalloc(VHOST_NET_VQ_MAX * sizeof(*vqs), GFP_KERNEL);
	if (!vqs) {
		kvfree(n);
		return -ENOMEM;
	}

	queue = kmalloc_array(VHOST_RX_BATCH, sizeof(void *),
			      GFP_KERNEL);
	if (!queue) {
		kfree(vqs);
		kvfree(n);
		return -ENOMEM;
	}
	n->vqs[VHOST_NET_VQ_RX].rxq.queue = queue;

	dev = &n->dev;
	vqs[VHOST_NET_VQ_TX] = &n->vqs[VHOST_NET_VQ_TX].vq;
	vqs[VHOST_NET_VQ_RX] = &n->vqs[VHOST_NET_VQ_RX].vq;
	n->vqs[VHOST_NET_VQ_TX].vq.handle_kick = handle_tx_kick;
	n->vqs[VHOST_NET_VQ_RX].vq.handle_kick = handle_rx_kick;
	for (i = 0; i < VHOST_NET_VQ_MAX; i++) {
		n->vqs[i].ubufs = NULL;
		n->vqs[i].ubuf_info = NULL;
		n->vqs[i].upend_idx = 0;
		n->vqs[i].done_idx = 0;
		n->vqs[i].vhost_hlen = 0;
		n->vqs[i].sock_hlen = 0;
		n->vqs[i].rx_ring = NULL;
		vhost_net_buf_init(&n->vqs[i].rxq);
	}
	vhost_dev_init(dev, vqs, VHOST_NET_VQ_MAX);

	vhost_poll_init(n->poll + VHOST_NET_VQ_TX, handle_tx_net, EPOLLOUT, dev);
	vhost_poll_init(n->poll + VHOST_NET_VQ_RX, handle_rx_net, EPOLLIN, dev);

	f->private_data = n;

	return 0;
}