vhost_user_set_mem_table(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd)
{
	struct virtio_net *dev = *pdev;
	struct VhostUserMemory *memory = &ctx->msg.payload.memory;
	struct rte_vhost_mem_region *reg;
	int numa_node = SOCKET_ID_ANY;
	uint64_t mmap_offset;
	uint32_t i;
	bool async_notify = false;

	if (validate_msg_fds(dev, ctx, memory->nregions) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	if (memory->nregions > VHOST_MEMORY_MAX_NREGIONS) {
		VHOST_LOG_CONFIG(ERR, "(%s) too many memory regions (%u)\n",
				dev->ifname, memory->nregions);
		goto close_msg_fds;
	}

	if (dev->mem && !vhost_memory_changed(memory, dev->mem)) {
		VHOST_LOG_CONFIG(INFO, "(%s) memory regions not changed\n", dev->ifname);

		close_msg_fds(ctx);

		return RTE_VHOST_MSG_RESULT_OK;
	}

	if (dev->mem) {
		if (dev->flags & VIRTIO_DEV_VDPA_CONFIGURED) {
			struct rte_vdpa_device *vdpa_dev = dev->vdpa_dev;

			if (vdpa_dev && vdpa_dev->ops->dev_close)
				vdpa_dev->ops->dev_close(dev->vid);
			dev->flags &= ~VIRTIO_DEV_VDPA_CONFIGURED;
		}

		/* notify the vhost application to stop DMA transfers */
		if (dev->async_copy && dev->notify_ops->vring_state_changed) {
			for (i = 0; i < dev->nr_vring; i++) {
				dev->notify_ops->vring_state_changed(dev->vid,
						i, 0);
			}
			async_notify = true;
		}

		free_mem_region(dev);
		rte_free(dev->mem);
		dev->mem = NULL;
	}

	/* Flush IOTLB cache as previous HVAs are now invalid */
	if (dev->features & (1ULL << VIRTIO_F_IOMMU_PLATFORM))
		for (i = 0; i < dev->nr_vring; i++)
			vhost_user_iotlb_flush_all(dev->virtqueue[i]);

	/*
	 * If VQ 0 has already been allocated, try to allocate on the same
	 * NUMA node. It can be reallocated later in numa_realloc().
	 */
	if (dev->nr_vring > 0)
		numa_node = dev->virtqueue[0]->numa_node;

	dev->nr_guest_pages = 0;
	if (dev->guest_pages == NULL) {
		dev->max_guest_pages = 8;
		dev->guest_pages = rte_zmalloc_socket(NULL,
					dev->max_guest_pages *
					sizeof(struct guest_page),
					RTE_CACHE_LINE_SIZE,
					numa_node);
		if (dev->guest_pages == NULL) {
			VHOST_LOG_CONFIG(ERR,
				"(%s) failed to allocate memory for dev->guest_pages\n",
				dev->ifname);
			goto close_msg_fds;
		}
	}

	dev->mem = rte_zmalloc_socket("vhost-mem-table", sizeof(struct rte_vhost_memory) +
		sizeof(struct rte_vhost_mem_region) * memory->nregions, 0, numa_node);
	if (dev->mem == NULL) {
		VHOST_LOG_CONFIG(ERR,
			"(%s) failed to allocate memory for dev->mem\n",
			dev->ifname);
		goto free_guest_pages;
	}

	for (i = 0; i < memory->nregions; i++) {
		reg = &dev->mem->regions[i];

		reg->guest_phys_addr = memory->regions[i].guest_phys_addr;
		reg->guest_user_addr = memory->regions[i].userspace_addr;
		reg->size            = memory->regions[i].memory_size;
		reg->fd              = ctx->fds[i];

		/*
		 * Assign invalid file descriptor value to avoid double
		 * closing on error path.
		 */
		ctx->fds[i] = -1;

		mmap_offset = memory->regions[i].mmap_offset;

		if (vhost_user_mmap_region(dev, reg, mmap_offset) < 0) {
			VHOST_LOG_CONFIG(ERR, "(%s) failed to mmap region %u\n", dev->ifname, i);
			goto free_mem_table;
		}

		dev->mem->nregions++;
	}

	if (dev->async_copy && rte_vfio_is_enabled("vfio"))
		async_dma_map(dev, true);

	if (vhost_user_postcopy_register(dev, main_fd, ctx) < 0)
		goto free_mem_table;

	for (i = 0; i < dev->nr_vring; i++) {
		struct vhost_virtqueue *vq = dev->virtqueue[i];

		if (!vq)
			continue;

		if (vq->desc || vq->avail || vq->used) {
			/*
			 * If the memory table got updated, the ring addresses
			 * need to be translated again as virtual addresses have
			 * changed.
			 */
			vring_invalidate(dev, vq);

			dev = translate_ring_addresses(dev, i);
			if (!dev) {
				dev = *pdev;
				goto free_mem_table;
			}

			*pdev = dev;
		}
	}

	dump_guest_pages(dev);

	if (async_notify) {
		for (i = 0; i < dev->nr_vring; i++)
			dev->notify_ops->vring_state_changed(dev->vid, i, 1);
	}

	return RTE_VHOST_MSG_RESULT_OK;

free_mem_table:
	free_mem_region(dev);
	rte_free(dev->mem);
	dev->mem = NULL;

free_guest_pages:
	rte_free(dev->guest_pages);
	dev->guest_pages = NULL;
close_msg_fds:
	close_msg_fds(ctx);
	return RTE_VHOST_MSG_RESULT_ERR;
}