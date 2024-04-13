static int process_slave_message_reply(struct virtio_net *dev,
				       const struct vhu_msg_context *ctx)
{
	struct vhu_msg_context msg_reply;
	int ret;

	if ((ctx->msg.flags & VHOST_USER_NEED_REPLY) == 0)
		return 0;

	ret = read_vhost_message(dev, dev->slave_req_fd, &msg_reply);
	if (ret <= 0) {
		if (ret < 0)
			VHOST_LOG_CONFIG(ERR, "(%s) vhost read slave message reply failed\n",
					dev->ifname);
		else
			VHOST_LOG_CONFIG(INFO, "(%s) vhost peer closed\n", dev->ifname);
		ret = -1;
		goto out;
	}

	ret = 0;
	if (msg_reply.msg.request.slave != ctx->msg.request.slave) {
		VHOST_LOG_CONFIG(ERR, "(%s) received unexpected msg type (%u), expected %u\n",
				dev->ifname, msg_reply.msg.request.slave, ctx->msg.request.slave);
		ret = -1;
		goto out;
	}

	ret = msg_reply.msg.payload.u64 ? -1 : 0;

out:
	rte_spinlock_unlock(&dev->slave_req_lock);
	return ret;
}