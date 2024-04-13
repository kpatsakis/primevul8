vhost_user_mmap_region(struct virtio_net *dev,
		struct rte_vhost_mem_region *region,
		uint64_t mmap_offset)
{
	void *mmap_addr;
	uint64_t mmap_size;
	uint64_t alignment;
	int populate;

	/* Check for memory_size + mmap_offset overflow */
	if (mmap_offset >= -region->size) {
		VHOST_LOG_CONFIG(ERR, "(%s) mmap_offset (%#"PRIx64") and memory_size (%#"PRIx64") overflow\n",
				dev->ifname, mmap_offset, region->size);
		return -1;
	}

	mmap_size = region->size + mmap_offset;

	/* mmap() without flag of MAP_ANONYMOUS, should be called with length
	 * argument aligned with hugepagesz at older longterm version Linux,
	 * like 2.6.32 and 3.2.72, or mmap() will fail with EINVAL.
	 *
	 * To avoid failure, make sure in caller to keep length aligned.
	 */
	alignment = get_blk_size(region->fd);
	if (alignment == (uint64_t)-1) {
		VHOST_LOG_CONFIG(ERR, "(%s) couldn't get hugepage size through fstat\n",
				dev->ifname);
		return -1;
	}
	mmap_size = RTE_ALIGN_CEIL(mmap_size, alignment);
	if (mmap_size == 0) {
		/*
		 * It could happen if initial mmap_size + alignment overflows
		 * the sizeof uint64, which could happen if either mmap_size or
		 * alignment value is wrong.
		 *
		 * mmap() kernel implementation would return an error, but
		 * better catch it before and provide useful info in the logs.
		 */
		VHOST_LOG_CONFIG(ERR, "(%s) mmap size (0x%" PRIx64 ") or alignment (0x%" PRIx64 ") is invalid\n",
				dev->ifname, region->size + mmap_offset, alignment);
		return -1;
	}

	populate = dev->async_copy ? MAP_POPULATE : 0;
	mmap_addr = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE,
			MAP_SHARED | populate, region->fd, 0);

	if (mmap_addr == MAP_FAILED) {
		VHOST_LOG_CONFIG(ERR, "(%s) mmap failed (%s).\n", dev->ifname, strerror(errno));
		return -1;
	}

	region->mmap_addr = mmap_addr;
	region->mmap_size = mmap_size;
	region->host_user_addr = (uint64_t)(uintptr_t)mmap_addr + mmap_offset;

	if (dev->async_copy) {
		if (add_guest_pages(dev, region, alignment) < 0) {
			VHOST_LOG_CONFIG(ERR, "(%s) adding guest pages to region failed.\n",
					dev->ifname);
			return -1;
		}
	}

	VHOST_LOG_CONFIG(INFO, "(%s) guest memory region size: 0x%" PRIx64 "\n",
			dev->ifname, region->size);
	VHOST_LOG_CONFIG(INFO, "(%s)\t guest physical addr: 0x%" PRIx64 "\n",
			dev->ifname, region->guest_phys_addr);
	VHOST_LOG_CONFIG(INFO, "(%s)\t guest virtual  addr: 0x%" PRIx64 "\n",
			dev->ifname, region->guest_user_addr);
	VHOST_LOG_CONFIG(INFO, "(%s)\t host  virtual  addr: 0x%" PRIx64 "\n",
			dev->ifname, region->host_user_addr);
	VHOST_LOG_CONFIG(INFO, "(%s)\t mmap addr : 0x%" PRIx64 "\n",
			dev->ifname, (uint64_t)(uintptr_t)mmap_addr);
	VHOST_LOG_CONFIG(INFO, "(%s)\t mmap size : 0x%" PRIx64 "\n",
			dev->ifname, mmap_size);
	VHOST_LOG_CONFIG(INFO, "(%s)\t mmap align: 0x%" PRIx64 "\n",
			dev->ifname, alignment);
	VHOST_LOG_CONFIG(INFO, "(%s)\t mmap off  : 0x%" PRIx64 "\n",
			dev->ifname, mmap_offset);

	return 0;
}