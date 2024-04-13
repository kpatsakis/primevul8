vhost_user_msg_handler(int vid, int fd)
{
	struct virtio_net *dev;
	struct vhu_msg_context ctx;
	struct rte_vdpa_device *vdpa_dev;
	int ret;
	int unlock_required = 0;
	bool handled;
	int request;
	uint32_t i;

	dev = get_device(vid);
	if (dev == NULL)
		return -1;

	if (!dev->notify_ops) {
		dev->notify_ops = vhost_driver_callback_get(dev->ifname);
		if (!dev->notify_ops) {
			VHOST_LOG_CONFIG(ERR, "(%s) failed to get callback ops for driver\n",
				dev->ifname);
			return -1;
		}
	}

	ret = read_vhost_message(dev, fd, &ctx);
	if (ret <= 0) {
		if (ret < 0)
			VHOST_LOG_CONFIG(ERR, "(%s) vhost read message failed\n", dev->ifname);
		else
			VHOST_LOG_CONFIG(INFO, "(%s) vhost peer closed\n", dev->ifname);

		return -1;
	}

	ret = 0;
	request = ctx.msg.request.master;
	if (request > VHOST_USER_NONE && request < VHOST_USER_MAX &&
			vhost_message_str[request]) {
		if (request != VHOST_USER_IOTLB_MSG)
			VHOST_LOG_CONFIG(INFO, "(%s) read message %s\n",
				dev->ifname, vhost_message_str[request]);
		else
			VHOST_LOG_CONFIG(DEBUG, "(%s) read message %s\n",
				dev->ifname, vhost_message_str[request]);
	} else {
		VHOST_LOG_CONFIG(DEBUG, "(%s) external request %d\n", dev->ifname, request);
	}

	ret = vhost_user_check_and_alloc_queue_pair(dev, &ctx);
	if (ret < 0) {
		VHOST_LOG_CONFIG(ERR, "(%s) failed to alloc queue\n", dev->ifname);
		return -1;
	}

	/*
	 * Note: we don't lock all queues on VHOST_USER_GET_VRING_BASE
	 * and VHOST_USER_RESET_OWNER, since it is sent when virtio stops
	 * and device is destroyed. destroy_device waits for queues to be
	 * inactive, so it is safe. Otherwise taking the access_lock
	 * would cause a dead lock.
	 */
	switch (request) {
	case VHOST_USER_SET_FEATURES:
	case VHOST_USER_SET_PROTOCOL_FEATURES:
	case VHOST_USER_SET_OWNER:
	case VHOST_USER_SET_MEM_TABLE:
	case VHOST_USER_SET_LOG_BASE:
	case VHOST_USER_SET_LOG_FD:
	case VHOST_USER_SET_VRING_NUM:
	case VHOST_USER_SET_VRING_ADDR:
	case VHOST_USER_SET_VRING_BASE:
	case VHOST_USER_SET_VRING_KICK:
	case VHOST_USER_SET_VRING_CALL:
	case VHOST_USER_SET_VRING_ERR:
	case VHOST_USER_SET_VRING_ENABLE:
	case VHOST_USER_SEND_RARP:
	case VHOST_USER_NET_SET_MTU:
	case VHOST_USER_SET_SLAVE_REQ_FD:
		if (!(dev->flags & VIRTIO_DEV_VDPA_CONFIGURED)) {
			vhost_user_lock_all_queue_pairs(dev);
			unlock_required = 1;
		}
		break;
	default:
		break;

	}

	handled = false;
	if (dev->extern_ops.pre_msg_handle) {
		RTE_BUILD_BUG_ON(offsetof(struct vhu_msg_context, msg) != 0);
		ret = (*dev->extern_ops.pre_msg_handle)(dev->vid, &ctx);
		switch (ret) {
		case RTE_VHOST_MSG_RESULT_REPLY:
			send_vhost_reply(dev, fd, &ctx);
			/* Fall-through */
		case RTE_VHOST_MSG_RESULT_ERR:
		case RTE_VHOST_MSG_RESULT_OK:
			handled = true;
			goto skip_to_post_handle;
		case RTE_VHOST_MSG_RESULT_NOT_HANDLED:
		default:
			break;
		}
	}

	if (request > VHOST_USER_NONE && request < VHOST_USER_MAX) {
		if (!vhost_message_handlers[request])
			goto skip_to_post_handle;
		ret = vhost_message_handlers[request](&dev, &ctx, fd);

		switch (ret) {
		case RTE_VHOST_MSG_RESULT_ERR:
			VHOST_LOG_CONFIG(ERR, "(%s) processing %s failed.\n",
					dev->ifname, vhost_message_str[request]);
			handled = true;
			break;
		case RTE_VHOST_MSG_RESULT_OK:
			VHOST_LOG_CONFIG(DEBUG, "(%s) processing %s succeeded.\n",
					dev->ifname, vhost_message_str[request]);
			handled = true;
			break;
		case RTE_VHOST_MSG_RESULT_REPLY:
			VHOST_LOG_CONFIG(DEBUG, "(%s) processing %s succeeded and needs reply.\n",
					dev->ifname, vhost_message_str[request]);
			send_vhost_reply(dev, fd, &ctx);
			handled = true;
			break;
		default:
			break;
		}
	}

skip_to_post_handle:
	if (ret != RTE_VHOST_MSG_RESULT_ERR &&
			dev->extern_ops.post_msg_handle) {
		RTE_BUILD_BUG_ON(offsetof(struct vhu_msg_context, msg) != 0);
		ret = (*dev->extern_ops.post_msg_handle)(dev->vid, &ctx);
		switch (ret) {
		case RTE_VHOST_MSG_RESULT_REPLY:
			send_vhost_reply(dev, fd, &ctx);
			/* Fall-through */
		case RTE_VHOST_MSG_RESULT_ERR:
		case RTE_VHOST_MSG_RESULT_OK:
			handled = true;
		case RTE_VHOST_MSG_RESULT_NOT_HANDLED:
		default:
			break;
		}
	}

	/* If message was not handled at this stage, treat it as an error */
	if (!handled) {
		VHOST_LOG_CONFIG(ERR, "(%s) vhost message (req: %d) was not handled.\n",
				dev->ifname, request);
		close_msg_fds(&ctx);
		ret = RTE_VHOST_MSG_RESULT_ERR;
	}

	/*
	 * If the request required a reply that was already sent,
	 * this optional reply-ack won't be sent as the
	 * VHOST_USER_NEED_REPLY was cleared in send_vhost_reply().
	 */
	if (ctx.msg.flags & VHOST_USER_NEED_REPLY) {
		ctx.msg.payload.u64 = ret == RTE_VHOST_MSG_RESULT_ERR;
		ctx.msg.size = sizeof(ctx.msg.payload.u64);
		ctx.fd_num = 0;
		send_vhost_reply(dev, fd, &ctx);
	} else if (ret == RTE_VHOST_MSG_RESULT_ERR) {
		VHOST_LOG_CONFIG(ERR, "(%s) vhost message handling failed.\n", dev->ifname);
		return -1;
	}

	for (i = 0; i < dev->nr_vring; i++) {
		struct vhost_virtqueue *vq = dev->virtqueue[i];
		bool cur_ready = vq_is_ready(dev, vq);

		if (cur_ready != (vq && vq->ready)) {
			vq->ready = cur_ready;
			vhost_user_notify_queue_state(dev, i, cur_ready);
		}
	}

	if (unlock_required)
		vhost_user_unlock_all_queue_pairs(dev);

	if (!virtio_is_ready(dev))
		goto out;

	/*
	 * Virtio is now ready. If not done already, it is time
	 * to notify the application it can process the rings and
	 * configure the vDPA device if present.
	 */

	if (!(dev->flags & VIRTIO_DEV_RUNNING)) {
		if (dev->notify_ops->new_device(dev->vid) == 0)
			dev->flags |= VIRTIO_DEV_RUNNING;
	}

	vdpa_dev = dev->vdpa_dev;
	if (!vdpa_dev)
		goto out;

	if (!(dev->flags & VIRTIO_DEV_VDPA_CONFIGURED)) {
		if (vdpa_dev->ops->dev_conf(dev->vid))
			VHOST_LOG_CONFIG(ERR, "(%s) failed to configure vDPA device\n",
					dev->ifname);
		else
			dev->flags |= VIRTIO_DEV_VDPA_CONFIGURED;
	}

out:
	return 0;
}