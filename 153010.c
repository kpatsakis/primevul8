vhost_user_postcopy_region_register(struct virtio_net *dev,
		struct rte_vhost_mem_region *reg)
{
	struct uffdio_register reg_struct;

	/*
	 * Let's register all the mmapped area to ensure
	 * alignment on page boundary.
	 */
	reg_struct.range.start = (uint64_t)(uintptr_t)reg->mmap_addr;
	reg_struct.range.len = reg->mmap_size;
	reg_struct.mode = UFFDIO_REGISTER_MODE_MISSING;

	if (ioctl(dev->postcopy_ufd, UFFDIO_REGISTER,
				&reg_struct)) {
		VHOST_LOG_CONFIG(ERR, "(%s) failed to register ufd for region "
				"%" PRIx64 " - %" PRIx64 " (ufd = %d) %s\n",
				dev->ifname,
				(uint64_t)reg_struct.range.start,
				(uint64_t)reg_struct.range.start +
				(uint64_t)reg_struct.range.len - 1,
				dev->postcopy_ufd,
				strerror(errno));
		return -1;
	}

	VHOST_LOG_CONFIG(INFO,
			"(%s)\t userfaultfd registered for range : %" PRIx64 " - %" PRIx64 "\n",
			dev->ifname,
			(uint64_t)reg_struct.range.start,
			(uint64_t)reg_struct.range.start +
			(uint64_t)reg_struct.range.len - 1);

	return 0;
}