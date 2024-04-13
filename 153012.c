send_vhost_slave_message(struct virtio_net *dev,
		struct vhu_msg_context *ctx)
{
	int ret;

	if (ctx->msg.flags & VHOST_USER_NEED_REPLY)
		rte_spinlock_lock(&dev->slave_req_lock);

	ret = send_vhost_message(dev, dev->slave_req_fd, ctx);
	if (ret < 0 && (ctx->msg.flags & VHOST_USER_NEED_REPLY))
		rte_spinlock_unlock(&dev->slave_req_lock);

	return ret;
}