static int vhost_user_slave_set_vring_host_notifier(struct virtio_net *dev,
						    int index, int fd,
						    uint64_t offset,
						    uint64_t size)
{
	int ret;
	struct vhu_msg_context ctx = {
		.msg = {
			.request.slave = VHOST_USER_SLAVE_VRING_HOST_NOTIFIER_MSG,
			.flags = VHOST_USER_VERSION | VHOST_USER_NEED_REPLY,
			.size = sizeof(ctx.msg.payload.area),
			.payload.area = {
				.u64 = index & VHOST_USER_VRING_IDX_MASK,
				.size = size,
				.offset = offset,
			},
		},
	};

	if (fd < 0)
		ctx.msg.payload.area.u64 |= VHOST_USER_VRING_NOFD_MASK;
	else {
		ctx.fds[0] = fd;
		ctx.fd_num = 1;
	}

	ret = send_vhost_slave_message(dev, &ctx);
	if (ret < 0) {
		VHOST_LOG_CONFIG(ERR, "(%s) failed to set host notifier (%d)\n",
				dev->ifname, ret);
		return ret;
	}

	return process_slave_message_reply(dev, &ctx);
}