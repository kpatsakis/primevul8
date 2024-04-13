add_guest_pages(struct virtio_net *dev, struct rte_vhost_mem_region *reg,
		uint64_t page_size)
{
	uint64_t reg_size = reg->size;
	uint64_t host_user_addr  = reg->host_user_addr;
	uint64_t guest_phys_addr = reg->guest_phys_addr;
	uint64_t host_iova;
	uint64_t size;

	host_iova = rte_mem_virt2iova((void *)(uintptr_t)host_user_addr);
	size = page_size - (guest_phys_addr & (page_size - 1));
	size = RTE_MIN(size, reg_size);

	if (add_one_guest_page(dev, guest_phys_addr, host_iova,
			       host_user_addr, size) < 0)
		return -1;

	host_user_addr  += size;
	guest_phys_addr += size;
	reg_size -= size;

	while (reg_size > 0) {
		size = RTE_MIN(reg_size, page_size);
		host_iova = rte_mem_virt2iova((void *)(uintptr_t)
						  host_user_addr);
		if (add_one_guest_page(dev, guest_phys_addr, host_iova,
				       host_user_addr, size) < 0)
			return -1;

		host_user_addr  += size;
		guest_phys_addr += size;
		reg_size -= size;
	}

	/* sort guest page array if over binary search threshold */
	if (dev->nr_guest_pages >= VHOST_BINARY_SEARCH_THRESH) {
		qsort((void *)dev->guest_pages, dev->nr_guest_pages,
			sizeof(struct guest_page), guest_page_addrcmp);
	}

	return 0;
}