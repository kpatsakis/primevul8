free_mem_region(struct virtio_net *dev)
{
	uint32_t i;
	struct rte_vhost_mem_region *reg;

	if (!dev || !dev->mem)
		return;

	if (dev->async_copy && rte_vfio_is_enabled("vfio"))
		async_dma_map(dev, false);

	for (i = 0; i < dev->mem->nregions; i++) {
		reg = &dev->mem->regions[i];
		if (reg->host_user_addr) {
			munmap(reg->mmap_addr, reg->mmap_size);
			close(reg->fd);
		}
	}
}