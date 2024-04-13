vhost_user_notify_queue_state(struct virtio_net *dev, uint16_t index,
			      int enable)
{
	struct rte_vdpa_device *vdpa_dev = dev->vdpa_dev;
	struct vhost_virtqueue *vq = dev->virtqueue[index];

	/* Configure guest notifications on enable */
	if (enable && vq->notif_enable != VIRTIO_UNINITIALIZED_NOTIF)
		vhost_enable_guest_notification(dev, vq, vq->notif_enable);

	if (vdpa_dev && vdpa_dev->ops->set_vring_state)
		vdpa_dev->ops->set_vring_state(dev->vid, index, enable);

	if (dev->notify_ops->vring_state_changed)
		dev->notify_ops->vring_state_changed(dev->vid,
				index, enable);
}