vhost_check_queue_inflights_packed(struct virtio_net *dev,
				   struct vhost_virtqueue *vq)
{
	uint16_t i;
	uint16_t resubmit_num = 0, old_used_idx, num;
	struct rte_vhost_resubmit_info *resubmit;
	struct rte_vhost_inflight_info_packed *inflight_packed;

	if (!(dev->protocol_features &
	    (1ULL << VHOST_USER_PROTOCOL_F_INFLIGHT_SHMFD)))
		return RTE_VHOST_MSG_RESULT_OK;

	/* The frontend may still not support the inflight feature
	 * although we negotiate the protocol feature.
	 */
	if ((!vq->inflight_packed))
		return RTE_VHOST_MSG_RESULT_OK;

	if (!vq->inflight_packed->version) {
		vq->inflight_packed->version = INFLIGHT_VERSION;
		return RTE_VHOST_MSG_RESULT_OK;
	}

	if (vq->resubmit_inflight)
		return RTE_VHOST_MSG_RESULT_OK;

	inflight_packed = vq->inflight_packed;
	vq->global_counter = 0;
	old_used_idx = inflight_packed->old_used_idx;

	if (inflight_packed->used_idx != old_used_idx) {
		if (inflight_packed->desc[old_used_idx].inflight == 0) {
			inflight_packed->old_used_idx =
				inflight_packed->used_idx;
			inflight_packed->old_used_wrap_counter =
				inflight_packed->used_wrap_counter;
			inflight_packed->old_free_head =
				inflight_packed->free_head;
		} else {
			inflight_packed->used_idx =
				inflight_packed->old_used_idx;
			inflight_packed->used_wrap_counter =
				inflight_packed->old_used_wrap_counter;
			inflight_packed->free_head =
				inflight_packed->old_free_head;
		}
	}

	for (i = 0; i < inflight_packed->desc_num; i++) {
		if (inflight_packed->desc[i].inflight == 1)
			resubmit_num++;
	}

	if (resubmit_num) {
		resubmit = rte_zmalloc_socket("resubmit", sizeof(struct rte_vhost_resubmit_info),
				0, vq->numa_node);
		if (resubmit == NULL) {
			VHOST_LOG_CONFIG(ERR,
					"(%s) failed to allocate memory for resubmit info.\n",
					dev->ifname);
			return RTE_VHOST_MSG_RESULT_ERR;
		}

		resubmit->resubmit_list = rte_zmalloc_socket("resubmit_list",
				resubmit_num * sizeof(struct rte_vhost_resubmit_desc),
				0, vq->numa_node);
		if (resubmit->resubmit_list == NULL) {
			VHOST_LOG_CONFIG(ERR,
					"(%s) failed to allocate memory for resubmit desc.\n",
					dev->ifname);
			rte_free(resubmit);
			return RTE_VHOST_MSG_RESULT_ERR;
		}

		num = 0;
		for (i = 0; i < inflight_packed->desc_num; i++) {
			if (vq->inflight_packed->desc[i].inflight == 1) {
				resubmit->resubmit_list[num].index = i;
				resubmit->resubmit_list[num].counter =
					inflight_packed->desc[i].counter;
				num++;
			}
		}
		resubmit->resubmit_num = num;

		if (resubmit->resubmit_num > 1)
			qsort(resubmit->resubmit_list, resubmit->resubmit_num,
			      sizeof(struct rte_vhost_resubmit_desc),
			      resubmit_desc_compare);

		vq->global_counter = resubmit->resubmit_list[0].counter + 1;
		vq->resubmit_inflight = resubmit;
	}

	return RTE_VHOST_MSG_RESULT_OK;
}