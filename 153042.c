virtio_is_ready(struct virtio_net *dev)
{
	struct vhost_virtqueue *vq;
	uint32_t i, nr_vring = dev->nr_vring;

	if (dev->flags & VIRTIO_DEV_READY)
		return 1;

	if (!dev->nr_vring)
		return 0;

	if (dev->flags & VIRTIO_DEV_BUILTIN_VIRTIO_NET) {
		nr_vring = VIRTIO_BUILTIN_NUM_VQS_TO_BE_READY;

		if (dev->nr_vring < nr_vring)
			return 0;
	}

	for (i = 0; i < nr_vring; i++) {
		vq = dev->virtqueue[i];

		if (!vq_is_ready(dev, vq))
			return 0;
	}

	/* If supported, ensure the frontend is really done with config */
	if (dev->protocol_features & (1ULL << VHOST_USER_PROTOCOL_F_STATUS))
		if (!(dev->status & VIRTIO_DEVICE_STATUS_DRIVER_OK))
			return 0;

	dev->flags |= VIRTIO_DEV_READY;

	if (!(dev->flags & VIRTIO_DEV_RUNNING))
		VHOST_LOG_CONFIG(INFO, "(%s) virtio is now ready for processing.\n", dev->ifname);
	return 1;
}