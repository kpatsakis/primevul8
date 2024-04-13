vhost_user_set_log_base(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	int fd = ctx->fds[0];
	uint64_t size, off;
	void *addr;
	uint32_t i;

	if (validate_msg_fds(dev, ctx, 1) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	if (fd < 0) {
		VHOST_LOG_CONFIG(ERR, "(%s) invalid log fd: %d\n", dev->ifname, fd);
		return RTE_VHOST_MSG_RESULT_ERR;
	}

	if (ctx->msg.size != sizeof(VhostUserLog)) {
		VHOST_LOG_CONFIG(ERR, "(%s) invalid log base msg size: %"PRId32" != %d\n",
			dev->ifname, ctx->msg.size, (int)sizeof(VhostUserLog));
		goto close_msg_fds;
	}

	size = ctx->msg.payload.log.mmap_size;
	off  = ctx->msg.payload.log.mmap_offset;

	/* Check for mmap size and offset overflow. */
	if (off >= -size) {
		VHOST_LOG_CONFIG(ERR,
				"(%s) log offset %#"PRIx64" and log size %#"PRIx64" overflow\n",
				dev->ifname, off, size);
		goto close_msg_fds;
	}

	VHOST_LOG_CONFIG(INFO, "(%s) log mmap size: %"PRId64", offset: %"PRId64"\n",
			dev->ifname, size, off);

	/*
	 * mmap from 0 to workaround a hugepage mmap bug: mmap will
	 * fail when offset is not page size aligned.
	 */
	addr = mmap(0, size + off, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (addr == MAP_FAILED) {
		VHOST_LOG_CONFIG(ERR, "(%s) mmap log base failed!\n", dev->ifname);
		return RTE_VHOST_MSG_RESULT_ERR;
	}

	/*
	 * Free previously mapped log memory on occasionally
	 * multiple VHOST_USER_SET_LOG_BASE.
	 */
	if (dev->log_addr) {
		munmap((void *)(uintptr_t)dev->log_addr, dev->log_size);
	}
	dev->log_addr = (uint64_t)(uintptr_t)addr;
	dev->log_base = dev->log_addr + off;
	dev->log_size = size;

	for (i = 0; i < dev->nr_vring; i++) {
		struct vhost_virtqueue *vq = dev->virtqueue[i];

		rte_free(vq->log_cache);
		vq->log_cache = NULL;
		vq->log_cache_nb_elem = 0;
		vq->log_cache = rte_malloc_socket("vq log cache",
				sizeof(struct log_cache_entry) * VHOST_LOG_CACHE_NR,
				0, vq->numa_node);
		/*
		 * If log cache alloc fail, don't fail migration, but no
		 * caching will be done, which will impact performance
		 */
		if (!vq->log_cache)
			VHOST_LOG_CONFIG(ERR, "(%s) failed to allocate VQ logging cache\n",
					dev->ifname);
	}

	/*
	 * The spec is not clear about it (yet), but QEMU doesn't expect
	 * any payload in the reply.
	 */
	ctx->msg.size = 0;
	ctx->fd_num = 0;

	return RTE_VHOST_MSG_RESULT_REPLY;

close_msg_fds:
	close_msg_fds(ctx);
	return RTE_VHOST_MSG_RESULT_ERR;
}