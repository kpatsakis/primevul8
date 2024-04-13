int rte_vhost_host_notifier_ctrl(int vid, uint16_t qid, bool enable)
{
	struct virtio_net *dev;
	struct rte_vdpa_device *vdpa_dev;
	int vfio_device_fd, ret = 0;
	uint64_t offset, size;
	unsigned int i, q_start, q_last;

	dev = get_device(vid);
	if (!dev)
		return -ENODEV;

	vdpa_dev = dev->vdpa_dev;
	if (vdpa_dev == NULL)
		return -ENODEV;

	if (!(dev->features & (1ULL << VIRTIO_F_VERSION_1)) ||
	    !(dev->features & (1ULL << VHOST_USER_F_PROTOCOL_FEATURES)) ||
	    !(dev->protocol_features &
			(1ULL << VHOST_USER_PROTOCOL_F_SLAVE_REQ)) ||
	    !(dev->protocol_features &
			(1ULL << VHOST_USER_PROTOCOL_F_SLAVE_SEND_FD)) ||
	    !(dev->protocol_features &
			(1ULL << VHOST_USER_PROTOCOL_F_HOST_NOTIFIER)))
		return -ENOTSUP;

	if (qid == RTE_VHOST_QUEUE_ALL) {
		q_start = 0;
		q_last = dev->nr_vring - 1;
	} else {
		if (qid >= dev->nr_vring)
			return -EINVAL;
		q_start = qid;
		q_last = qid;
	}

	RTE_FUNC_PTR_OR_ERR_RET(vdpa_dev->ops->get_vfio_device_fd, -ENOTSUP);
	RTE_FUNC_PTR_OR_ERR_RET(vdpa_dev->ops->get_notify_area, -ENOTSUP);

	vfio_device_fd = vdpa_dev->ops->get_vfio_device_fd(vid);
	if (vfio_device_fd < 0)
		return -ENOTSUP;

	if (enable) {
		for (i = q_start; i <= q_last; i++) {
			if (vdpa_dev->ops->get_notify_area(vid, i, &offset,
					&size) < 0) {
				ret = -ENOTSUP;
				goto disable;
			}

			if (vhost_user_slave_set_vring_host_notifier(dev, i,
					vfio_device_fd, offset, size) < 0) {
				ret = -EFAULT;
				goto disable;
			}
		}
	} else {
disable:
		for (i = q_start; i <= q_last; i++) {
			vhost_user_slave_set_vring_host_notifier(dev, i, -1,
					0, 0);
		}
	}

	return ret;
}