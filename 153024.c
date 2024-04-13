vhost_user_set_vring_base(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	struct vhost_virtqueue *vq = dev->virtqueue[ctx->msg.payload.state.index];
	uint64_t val = ctx->msg.payload.state.num;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	if (vq_is_packed(dev)) {
		/*
		 * Bit[0:14]: avail index
		 * Bit[15]: avail wrap counter
		 */
		vq->last_avail_idx = val & 0x7fff;
		vq->avail_wrap_counter = !!(val & (0x1 << 15));
		/*
		 * Set used index to same value as available one, as
		 * their values should be the same since ring processing
		 * was stopped at get time.
		 */
		vq->last_used_idx = vq->last_avail_idx;
		vq->used_wrap_counter = vq->avail_wrap_counter;
	} else {
		vq->last_used_idx = ctx->msg.payload.state.num;
		vq->last_avail_idx = ctx->msg.payload.state.num;
	}

	VHOST_LOG_CONFIG(INFO,
		"(%s) vring base idx:%u last_used_idx:%u last_avail_idx:%u.\n",
		dev->ifname, ctx->msg.payload.state.index, vq->last_used_idx,
		vq->last_avail_idx);

	return RTE_VHOST_MSG_RESULT_OK;
}