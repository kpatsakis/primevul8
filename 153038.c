async_dma_map(struct virtio_net *dev, bool do_map)
{
	int ret = 0;
	uint32_t i;
	struct guest_page *page;

	if (do_map) {
		for (i = 0; i < dev->nr_guest_pages; i++) {
			page = &dev->guest_pages[i];
			ret = rte_vfio_container_dma_map(RTE_VFIO_DEFAULT_CONTAINER_FD,
							 page->host_user_addr,
							 page->host_iova,
							 page->size);
			if (ret) {
				/*
				 * DMA device may bind with kernel driver, in this case,
				 * we don't need to program IOMMU manually. However, if no
				 * device is bound with vfio/uio in DPDK, and vfio kernel
				 * module is loaded, the API will still be called and return
				 * with ENODEV.
				 *
				 * DPDK vfio only returns ENODEV in very similar situations
				 * (vfio either unsupported, or supported but no devices found).
				 * Either way, no mappings could be performed. We treat it as
				 * normal case in async path. This is a workaround.
				 */
				if (rte_errno == ENODEV)
					return;

				/* DMA mapping errors won't stop VHOST_USER_SET_MEM_TABLE. */
				VHOST_LOG_CONFIG(ERR, "DMA engine map failed\n");
			}
		}

	} else {
		for (i = 0; i < dev->nr_guest_pages; i++) {
			page = &dev->guest_pages[i];
			ret = rte_vfio_container_dma_unmap(RTE_VFIO_DEFAULT_CONTAINER_FD,
							   page->host_user_addr,
							   page->host_iova,
							   page->size);
			if (ret) {
				/* like DMA map, ignore the kernel driver case when unmap. */
				if (rte_errno == EINVAL)
					return;

				VHOST_LOG_CONFIG(ERR, "DMA engine unmap failed\n");
			}
		}
	}
}