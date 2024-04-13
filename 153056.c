vhost_user_set_vring_addr(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	struct vhost_virtqueue *vq;
	struct vhost_vring_addr *addr = &ctx->msg.payload.addr;
	bool access_ok;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	if (dev->mem == NULL)
		return RTE_VHOST_MSG_RESULT_ERR;

	/* addr->index refers to the queue index. The txq 1, rxq is 0. */
	vq = dev->virtqueue[ctx->msg.payload.addr.index];

	access_ok = vq->access_ok;

	/*
	 * Rings addresses should not be interpreted as long as the ring is not
	 * started and enabled
	 */
	memcpy(&vq->ring_addrs, addr, sizeof(*addr));

	vring_invalidate(dev, vq);

	if ((vq->enabled && (dev->features &
				(1ULL << VHOST_USER_F_PROTOCOL_FEATURES))) ||
			access_ok) {
		dev = translate_ring_addresses(dev, ctx->msg.payload.addr.index);
		if (!dev)
			return RTE_VHOST_MSG_RESULT_ERR;

		*pdev = dev;
	}

	return RTE_VHOST_MSG_RESULT_OK;
}