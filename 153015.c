vhost_user_iotlb_miss(struct virtio_net *dev, uint64_t iova, uint8_t perm)
{
	int ret;
	struct vhu_msg_context ctx = {
		.msg = {
			.request.slave = VHOST_USER_SLAVE_IOTLB_MSG,
			.flags = VHOST_USER_VERSION,
			.size = sizeof(ctx.msg.payload.iotlb),
			.payload.iotlb = {
				.iova = iova,
				.perm = perm,
				.type = VHOST_IOTLB_MISS,
			},
		},
	};

	ret = send_vhost_message(dev, dev->slave_req_fd, &ctx);
	if (ret < 0) {
		VHOST_LOG_CONFIG(ERR, "(%s) failed to send IOTLB miss message (%d)\n",
				dev->ifname, ret);
		return ret;
	}

	return 0;
}