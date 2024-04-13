vhost_user_postcopy_register(struct virtio_net *dev, int main_fd,
		struct vhu_msg_context *ctx)
{
	struct VhostUserMemory *memory;
	struct rte_vhost_mem_region *reg;
	struct vhu_msg_context ack_ctx;
	uint32_t i;

	if (!dev->postcopy_listening)
		return 0;

	/*
	 * We haven't a better way right now than sharing
	 * DPDK's virtual address with Qemu, so that Qemu can
	 * retrieve the region offset when handling userfaults.
	 */
	memory = &ctx->msg.payload.memory;
	for (i = 0; i < memory->nregions; i++) {
		reg = &dev->mem->regions[i];
		memory->regions[i].userspace_addr = reg->host_user_addr;
	}

	/* Send the addresses back to qemu */
	ctx->fd_num = 0;
	send_vhost_reply(dev, main_fd, ctx);

	/* Wait for qemu to acknowledge it got the addresses
	 * we've got to wait before we're allowed to generate faults.
	 */
	if (read_vhost_message(dev, main_fd, &ack_ctx) <= 0) {
		VHOST_LOG_CONFIG(ERR, "(%s) failed to read qemu ack on postcopy set-mem-table\n",
				dev->ifname);
		return -1;
	}

	if (validate_msg_fds(dev, &ack_ctx, 0) != 0)
		return -1;

	if (ack_ctx.msg.request.master != VHOST_USER_SET_MEM_TABLE) {
		VHOST_LOG_CONFIG(ERR, "(%s) bad qemu ack on postcopy set-mem-table (%d)\n",
				dev->ifname, ack_ctx.msg.request.master);
		return -1;
	}

	/* Now userfault register and we can use the memory */
	for (i = 0; i < memory->nregions; i++) {
		reg = &dev->mem->regions[i];
		if (vhost_user_postcopy_region_register(dev, reg) < 0)
			return -1;
	}

	return 0;
}