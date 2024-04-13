vhost_user_set_status(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	/* As per Virtio specification, the device status is 8bits long */
	if (ctx->msg.payload.u64 > UINT8_MAX) {
		VHOST_LOG_CONFIG(ERR, "(%s) invalid VHOST_USER_SET_STATUS payload 0x%" PRIx64 "\n",
				dev->ifname, ctx->msg.payload.u64);
		return RTE_VHOST_MSG_RESULT_ERR;
	}

	dev->status = ctx->msg.payload.u64;

	if ((dev->status & VIRTIO_DEVICE_STATUS_FEATURES_OK) &&
	    (dev->flags & VIRTIO_DEV_FEATURES_FAILED)) {
		VHOST_LOG_CONFIG(ERR,
				"(%s) FEATURES_OK bit is set but feature negotiation failed\n",
				dev->ifname);
		/*
		 * Clear the bit to let the driver know about the feature
		 * negotiation failure
		 */
		dev->status &= ~VIRTIO_DEVICE_STATUS_FEATURES_OK;
	}

	VHOST_LOG_CONFIG(INFO, "(%s) new device status(0x%08x):\n", dev->ifname,
			dev->status);
	VHOST_LOG_CONFIG(INFO, "(%s)\t-RESET: %u\n", dev->ifname,
			(dev->status == VIRTIO_DEVICE_STATUS_RESET));
	VHOST_LOG_CONFIG(INFO, "(%s)\t-ACKNOWLEDGE: %u\n", dev->ifname,
			!!(dev->status & VIRTIO_DEVICE_STATUS_ACK));
	VHOST_LOG_CONFIG(INFO, "(%s)\t-DRIVER: %u\n", dev->ifname,
			!!(dev->status & VIRTIO_DEVICE_STATUS_DRIVER));
	VHOST_LOG_CONFIG(INFO, "(%s)\t-FEATURES_OK: %u\n", dev->ifname,
			!!(dev->status & VIRTIO_DEVICE_STATUS_FEATURES_OK));
	VHOST_LOG_CONFIG(INFO, "(%s)\t-DRIVER_OK: %u\n", dev->ifname,
			!!(dev->status & VIRTIO_DEVICE_STATUS_DRIVER_OK));
	VHOST_LOG_CONFIG(INFO, "(%s)\t-DEVICE_NEED_RESET: %u\n", dev->ifname,
			!!(dev->status & VIRTIO_DEVICE_STATUS_DEV_NEED_RESET));
	VHOST_LOG_CONFIG(INFO, "(%s)\t-FAILED: %u\n", dev->ifname,
			!!(dev->status & VIRTIO_DEVICE_STATUS_FAILED));

	return RTE_VHOST_MSG_RESULT_OK;
}