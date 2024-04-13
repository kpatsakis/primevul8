vhost_user_iotlb_msg(struct virtio_net **pdev,
			struct vhu_msg_context *ctx,
			int main_fd __rte_unused)
{
	struct virtio_net *dev = *pdev;
	struct vhost_iotlb_msg *imsg = &ctx->msg.payload.iotlb;
	uint16_t i;
	uint64_t vva, len;

	if (validate_msg_fds(dev, ctx, 0) != 0)
		return RTE_VHOST_MSG_RESULT_ERR;

	switch (imsg->type) {
	case VHOST_IOTLB_UPDATE:
		len = imsg->size;
		vva = qva_to_vva(dev, imsg->uaddr, &len);
		if (!vva)
			return RTE_VHOST_MSG_RESULT_ERR;

		for (i = 0; i < dev->nr_vring; i++) {
			struct vhost_virtqueue *vq = dev->virtqueue[i];

			if (!vq)
				continue;

			vhost_user_iotlb_cache_insert(dev, vq, imsg->iova, vva,
					len, imsg->perm);

			if (is_vring_iotlb(dev, vq, imsg)) {
				rte_spinlock_lock(&vq->access_lock);
				*pdev = dev = translate_ring_addresses(dev, i);
				rte_spinlock_unlock(&vq->access_lock);
			}
		}
		break;
	case VHOST_IOTLB_INVALIDATE:
		for (i = 0; i < dev->nr_vring; i++) {
			struct vhost_virtqueue *vq = dev->virtqueue[i];

			if (!vq)
				continue;

			vhost_user_iotlb_cache_remove(vq, imsg->iova,
					imsg->size);

			if (is_vring_iotlb(dev, vq, imsg)) {
				rte_spinlock_lock(&vq->access_lock);
				vring_invalidate(dev, vq);
				rte_spinlock_unlock(&vq->access_lock);
			}
		}
		break;
	default:
		VHOST_LOG_CONFIG(ERR, "(%s) invalid IOTLB message type (%d)\n",
				dev->ifname, imsg->type);
		return RTE_VHOST_MSG_RESULT_ERR;
	}

	return RTE_VHOST_MSG_RESULT_OK;
}