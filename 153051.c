numa_realloc(struct virtio_net *dev, int index)
{
	int node, dev_node;
	struct virtio_net *old_dev;
	struct vhost_virtqueue *vq;
	struct batch_copy_elem *bce;
	struct guest_page *gp;
	struct rte_vhost_memory *mem;
	size_t mem_size;
	int ret;

	old_dev = dev;
	vq = dev->virtqueue[index];

	/*
	 * If VQ is ready, it is too late to reallocate, it certainly already
	 * happened anyway on VHOST_USER_SET_VRING_ADRR.
	 */
	if (vq->ready)
		return dev;

	ret = get_mempolicy(&node, NULL, 0, vq->desc, MPOL_F_NODE | MPOL_F_ADDR);
	if (ret) {
		VHOST_LOG_CONFIG(ERR, "(%s) unable to get virtqueue %d numa information.\n",
				dev->ifname, index);
		return dev;
	}

	if (node == vq->numa_node)
		goto out_dev_realloc;

	vq = rte_realloc_socket(vq, sizeof(*vq), 0, node);
	if (!vq) {
		VHOST_LOG_CONFIG(ERR, "(%s) failed to realloc virtqueue %d on node %d\n",
				dev->ifname, index, node);
		return dev;
	}

	if (vq != dev->virtqueue[index]) {
		VHOST_LOG_CONFIG(INFO, "(%s) reallocated virtqueue on node %d\n",
				dev->ifname, node);
		dev->virtqueue[index] = vq;
		vhost_user_iotlb_init(dev, index);
	}

	if (vq_is_packed(dev)) {
		struct vring_used_elem_packed *sup;

		sup = rte_realloc_socket(vq->shadow_used_packed, vq->size * sizeof(*sup),
				RTE_CACHE_LINE_SIZE, node);
		if (!sup) {
			VHOST_LOG_CONFIG(ERR, "(%s) failed to realloc shadow packed on node %d\n",
					dev->ifname, node);
			return dev;
		}
		vq->shadow_used_packed = sup;
	} else {
		struct vring_used_elem *sus;

		sus = rte_realloc_socket(vq->shadow_used_split, vq->size * sizeof(*sus),
				RTE_CACHE_LINE_SIZE, node);
		if (!sus) {
			VHOST_LOG_CONFIG(ERR, "(%s) failed to realloc shadow split on node %d\n",
					dev->ifname, node);
			return dev;
		}
		vq->shadow_used_split = sus;
	}

	bce = rte_realloc_socket(vq->batch_copy_elems, vq->size * sizeof(*bce),
			RTE_CACHE_LINE_SIZE, node);
	if (!bce) {
		VHOST_LOG_CONFIG(ERR, "(%s) failed to realloc batch copy elem on node %d\n",
				dev->ifname, node);
		return dev;
	}
	vq->batch_copy_elems = bce;

	if (vq->log_cache) {
		struct log_cache_entry *lc;

		lc = rte_realloc_socket(vq->log_cache, sizeof(*lc) * VHOST_LOG_CACHE_NR, 0, node);
		if (!lc) {
			VHOST_LOG_CONFIG(ERR, "(%s) failed to realloc log cache on node %d\n",
					dev->ifname, node);
			return dev;
		}
		vq->log_cache = lc;
	}

	if (vq->resubmit_inflight) {
		struct rte_vhost_resubmit_info *ri;

		ri = rte_realloc_socket(vq->resubmit_inflight, sizeof(*ri), 0, node);
		if (!ri) {
			VHOST_LOG_CONFIG(ERR, "(%s) failed to realloc resubmit inflight on node %d\n",
					dev->ifname, node);
			return dev;
		}
		vq->resubmit_inflight = ri;

		if (ri->resubmit_list) {
			struct rte_vhost_resubmit_desc *rd;

			rd = rte_realloc_socket(ri->resubmit_list, sizeof(*rd) * ri->resubmit_num,
					0, node);
			if (!rd) {
				VHOST_LOG_CONFIG(ERR, "(%s) failed to realloc resubmit list on node %d\n",
						dev->ifname, node);
				return dev;
			}
			ri->resubmit_list = rd;
		}
	}

	vq->numa_node = node;

out_dev_realloc:

	if (dev->flags & VIRTIO_DEV_RUNNING)
		return dev;

	ret = get_mempolicy(&dev_node, NULL, 0, dev, MPOL_F_NODE | MPOL_F_ADDR);
	if (ret) {
		VHOST_LOG_CONFIG(ERR, "(%s) unable to get numa information.\n", dev->ifname);
		return dev;
	}

	if (dev_node == node)
		return dev;

	dev = rte_realloc_socket(old_dev, sizeof(*dev), 0, node);
	if (!dev) {
		VHOST_LOG_CONFIG(ERR, "(%s) failed to realloc dev on node %d\n",
				old_dev->ifname, node);
		return old_dev;
	}

	VHOST_LOG_CONFIG(INFO, "(%s) reallocated device on node %d\n", dev->ifname, node);
	vhost_devices[dev->vid] = dev;

	mem_size = sizeof(struct rte_vhost_memory) +
		sizeof(struct rte_vhost_mem_region) * dev->mem->nregions;
	mem = rte_realloc_socket(dev->mem, mem_size, 0, node);
	if (!mem) {
		VHOST_LOG_CONFIG(ERR, "(%s) failed to realloc mem table on node %d\n",
				dev->ifname, node);
		return dev;
	}
	dev->mem = mem;

	gp = rte_realloc_socket(dev->guest_pages, dev->max_guest_pages * sizeof(*gp),
			RTE_CACHE_LINE_SIZE, node);
	if (!gp) {
		VHOST_LOG_CONFIG(ERR, "(%s) failed to realloc guest pages on node %d\n",
				dev->ifname, node);
		return dev;
	}
	dev->guest_pages = gp;

	return dev;
}