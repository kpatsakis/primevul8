vhost_user_send_rarp(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	uint8_t *mac = (uint8_t *)&ctx->msg.payload.u64;
	struct rte_vdpa_device *vdpa_dev;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	VHOST_LOG_CONFIG(DEBUG, "(%s) MAC: " RTE_ETHER_ADDR_PRT_FMT "\n",
		dev->ifname, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	memcpy(dev->mac.addr_bytes, mac, 6);

	/*
	 * Set the flag to inject a RARP broadcast packet at
	 * rte_vhost_dequeue_burst().
	 *
	 * __ATOMIC_RELEASE ordering is for making sure the mac is
	 * copied before the flag is set.
	 */
	__atomic_store_n(&dev->broadcast_rarp, 1, __ATOMIC_RELEASE);
	vdpa_dev = dev->vdpa_dev;
	if (vdpa_dev && vdpa_dev->ops->migration_done)
		vdpa_dev->ops->migration_done(dev->vid);

	return RTE_VHOST_MSG_RESULT_OK;
}