vhost_user_set_vring_num(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	struct vhost_virtqueue *vq = dev->virtqueue[ctx->msg.payload.state.index];

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	if (ctx->msg.payload.state.num > 32768) {
		VHOST_LOG_CONFIG(ERR, "(%s) invalid virtqueue size %u\n",
				dev->ifname, ctx->msg.payload.state.num);
		return RTE_VHOST_MSG_RESULT_ERR;
	}

	vq->size = ctx->msg.payload.state.num;

	/* VIRTIO 1.0, 2.4 Virtqueues says:
	 *
	 *   Queue Size value is always a power of 2. The maximum Queue Size
	 *   value is 32768.
	 *
	 * VIRTIO 1.1 2.7 Virtqueues says:
	 *
	 *   Packed virtqueues support up to 2^15 entries each.
	 */
	if (!vq_is_packed(dev)) {
		if (vq->size & (vq->size - 1)) {
			VHOST_LOG_CONFIG(ERR, "(%s) invalid virtqueue size %u\n",
					dev->ifname, vq->size);
			return RTE_VHOST_MSG_RESULT_ERR;
		}
	}

	if (vq_is_packed(dev)) {
		rte_free(vq->shadow_used_packed);
		vq->shadow_used_packed = rte_malloc_socket(NULL,
				vq->size *
				sizeof(struct vring_used_elem_packed),
				RTE_CACHE_LINE_SIZE, vq->numa_node);
		if (!vq->shadow_used_packed) {
			VHOST_LOG_CONFIG(ERR,
				"(%s) failed to allocate memory for shadow used ring.\n",
				dev->ifname);
			return RTE_VHOST_MSG_RESULT_ERR;
		}

	} else {
		rte_free(vq->shadow_used_split);

		vq->shadow_used_split = rte_malloc_socket(NULL,
				vq->size * sizeof(struct vring_used_elem),
				RTE_CACHE_LINE_SIZE, vq->numa_node);

		if (!vq->shadow_used_split) {
			VHOST_LOG_CONFIG(ERR,
				"(%s) failed to allocate memory for vq internal data.\n",
				dev->ifname);
			return RTE_VHOST_MSG_RESULT_ERR;
		}
	}

	rte_free(vq->batch_copy_elems);
	vq->batch_copy_elems = rte_malloc_socket(NULL,
				vq->size * sizeof(struct batch_copy_elem),
				RTE_CACHE_LINE_SIZE, vq->numa_node);
	if (!vq->batch_copy_elems) {
		VHOST_LOG_CONFIG(ERR, "(%s) failed to allocate memory for batching copy.\n",
			dev->ifname);
		return RTE_VHOST_MSG_RESULT_ERR;
	}

	return RTE_VHOST_MSG_RESULT_OK;
}