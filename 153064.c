translate_ring_addresses(struct virtio_net *dev, int vq_index)
{
	struct vhost_virtqueue *vq = dev->virtqueue[vq_index];
	struct vhost_vring_addr *addr = &vq->ring_addrs;
	uint64_t len, expected_len;

	if (addr->flags & (1 << VHOST_VRING_F_LOG)) {
		vq->log_guest_addr =
			log_addr_to_gpa(dev, vq);
		if (vq->log_guest_addr == 0) {
			VHOST_LOG_CONFIG(DEBUG, "(%s) failed to map log_guest_addr.\n",
				dev->ifname);
			return dev;
		}
	}

	if (vq_is_packed(dev)) {
		len = sizeof(struct vring_packed_desc) * vq->size;
		vq->desc_packed = (struct vring_packed_desc *)(uintptr_t)
			ring_addr_to_vva(dev, vq, addr->desc_user_addr, &len);
		if (vq->desc_packed == NULL ||
				len != sizeof(struct vring_packed_desc) *
				vq->size) {
			VHOST_LOG_CONFIG(DEBUG, "(%s) failed to map desc_packed ring.\n",
				dev->ifname);
			return dev;
		}

		dev = numa_realloc(dev, vq_index);
		vq = dev->virtqueue[vq_index];
		addr = &vq->ring_addrs;

		len = sizeof(struct vring_packed_desc_event);
		vq->driver_event = (struct vring_packed_desc_event *)
					(uintptr_t)ring_addr_to_vva(dev,
					vq, addr->avail_user_addr, &len);
		if (vq->driver_event == NULL ||
				len != sizeof(struct vring_packed_desc_event)) {
			VHOST_LOG_CONFIG(DEBUG, "(%s) failed to find driver area address.\n",
				dev->ifname);
			return dev;
		}

		len = sizeof(struct vring_packed_desc_event);
		vq->device_event = (struct vring_packed_desc_event *)
					(uintptr_t)ring_addr_to_vva(dev,
					vq, addr->used_user_addr, &len);
		if (vq->device_event == NULL ||
				len != sizeof(struct vring_packed_desc_event)) {
			VHOST_LOG_CONFIG(DEBUG, "(%s) failed to find device area address.\n",
				dev->ifname);
			return dev;
		}

		vq->access_ok = true;
		return dev;
	}

	/* The addresses are converted from QEMU virtual to Vhost virtual. */
	if (vq->desc && vq->avail && vq->used)
		return dev;

	len = sizeof(struct vring_desc) * vq->size;
	vq->desc = (struct vring_desc *)(uintptr_t)ring_addr_to_vva(dev,
			vq, addr->desc_user_addr, &len);
	if (vq->desc == 0 || len != sizeof(struct vring_desc) * vq->size) {
		VHOST_LOG_CONFIG(DEBUG, "(%s) failed to map desc ring.\n", dev->ifname);
		return dev;
	}

	dev = numa_realloc(dev, vq_index);
	vq = dev->virtqueue[vq_index];
	addr = &vq->ring_addrs;

	len = sizeof(struct vring_avail) + sizeof(uint16_t) * vq->size;
	if (dev->features & (1ULL << VIRTIO_RING_F_EVENT_IDX))
		len += sizeof(uint16_t);
	expected_len = len;
	vq->avail = (struct vring_avail *)(uintptr_t)ring_addr_to_vva(dev,
			vq, addr->avail_user_addr, &len);
	if (vq->avail == 0 || len != expected_len) {
		VHOST_LOG_CONFIG(DEBUG, "(%s) failed to map avail ring.\n", dev->ifname);
		return dev;
	}

	len = sizeof(struct vring_used) +
		sizeof(struct vring_used_elem) * vq->size;
	if (dev->features & (1ULL << VIRTIO_RING_F_EVENT_IDX))
		len += sizeof(uint16_t);
	expected_len = len;
	vq->used = (struct vring_used *)(uintptr_t)ring_addr_to_vva(dev,
			vq, addr->used_user_addr, &len);
	if (vq->used == 0 || len != expected_len) {
		VHOST_LOG_CONFIG(DEBUG, "(%s) failed to map used ring.\n", dev->ifname);
		return dev;
	}

	if (vq->last_used_idx != vq->used->idx) {
		VHOST_LOG_CONFIG(WARNING, "(%s) last_used_idx (%u) and vq->used->idx (%u) mismatches;\n",
			dev->ifname,
			vq->last_used_idx, vq->used->idx);
		vq->last_used_idx  = vq->used->idx;
		vq->last_avail_idx = vq->used->idx;
		VHOST_LOG_CONFIG(WARNING, "(%s) some packets maybe resent for Tx and dropped for Rx\n",
			dev->ifname);
	}

	vq->access_ok = true;

	VHOST_LOG_CONFIG(DEBUG, "(%s) mapped address desc: %p\n", dev->ifname, vq->desc);
	VHOST_LOG_CONFIG(DEBUG, "(%s) mapped address avail: %p\n", dev->ifname, vq->avail);
	VHOST_LOG_CONFIG(DEBUG, "(%s) mapped address used: %p\n", dev->ifname, vq->used);
	VHOST_LOG_CONFIG(DEBUG, "(%s) log_guest_addr: %" PRIx64 "\n",
			dev->ifname, vq->log_guest_addr);

	return dev;
}