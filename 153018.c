vhost_user_set_features(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	uint64_t features = ctx->msg.payload.u64;
	uint64_t vhost_features = 0;
	struct rte_vdpa_device *vdpa_dev;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	rte_vhost_driver_get_features(dev->ifname, &vhost_features);
	if (features & ~vhost_features) {
		VHOST_LOG_CONFIG(ERR, "(%s) received invalid negotiated features.\n",
			dev->ifname);
		dev->flags |= VIRTIO_DEV_FEATURES_FAILED;
		dev->status &= ~VIRTIO_DEVICE_STATUS_FEATURES_OK;

		return RTE_VHOST_MSG_RESULT_ERR;
	}

	if (dev->flags & VIRTIO_DEV_RUNNING) {
		if (dev->features == features)
			return RTE_VHOST_MSG_RESULT_OK;

		/*
		 * Error out if master tries to change features while device is
		 * in running state. The exception being VHOST_F_LOG_ALL, which
		 * is enabled when the live-migration starts.
		 */
		if ((dev->features ^ features) & ~(1ULL << VHOST_F_LOG_ALL)) {
			VHOST_LOG_CONFIG(ERR, "(%s) features changed while device is running.\n",
				dev->ifname);
			return RTE_VHOST_MSG_RESULT_ERR;
		}

		if (dev->notify_ops->features_changed)
			dev->notify_ops->features_changed(dev->vid, features);
	}

	dev->features = features;
	if (dev->features &
		((1ULL << VIRTIO_NET_F_MRG_RXBUF) |
		 (1ULL << VIRTIO_F_VERSION_1) |
		 (1ULL << VIRTIO_F_RING_PACKED))) {
		dev->vhost_hlen = sizeof(struct virtio_net_hdr_mrg_rxbuf);
	} else {
		dev->vhost_hlen = sizeof(struct virtio_net_hdr);
	}
	VHOST_LOG_CONFIG(INFO, "(%s) negotiated Virtio features: 0x%" PRIx64 "\n",
			dev->ifname, dev->features);
	VHOST_LOG_CONFIG(DEBUG, "(%s) mergeable RX buffers %s, virtio 1 %s\n",
		dev->ifname,
		(dev->features & (1 << VIRTIO_NET_F_MRG_RXBUF)) ? "on" : "off",
		(dev->features & (1ULL << VIRTIO_F_VERSION_1)) ? "on" : "off");

	if ((dev->flags & VIRTIO_DEV_BUILTIN_VIRTIO_NET) &&
	    !(dev->features & (1ULL << VIRTIO_NET_F_MQ))) {
		/*
		 * Remove all but first queue pair if MQ hasn't been
		 * negotiated. This is safe because the device is not
		 * running at this stage.
		 */
		while (dev->nr_vring > 2) {
			struct vhost_virtqueue *vq;

			vq = dev->virtqueue[--dev->nr_vring];
			if (!vq)
				continue;

			dev->virtqueue[dev->nr_vring] = NULL;
			cleanup_vq(vq, 1);
			cleanup_vq_inflight(dev, vq);
			free_vq(dev, vq);
		}
	}

	vdpa_dev = dev->vdpa_dev;
	if (vdpa_dev)
		vdpa_dev->ops->set_features(dev->vid);

	dev->flags &= ~VIRTIO_DEV_FEATURES_FAILED;
	return RTE_VHOST_MSG_RESULT_OK;
}