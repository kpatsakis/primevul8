vhost_check_queue_inflights_split(struct virtio_net *dev,
				  struct vhost_virtqueue *vq)
{
	uint16_t i;
	uint16_t resubmit_num = 0, last_io, num;
	struct vring_used *used = vq->used;
	struct rte_vhost_resubmit_info *resubmit;
	struct rte_vhost_inflight_info_split *inflight_split;

	if (!(dev->protocol_features &
	    (1ULL << VHOST_USER_PROTOCOL_F_INFLIGHT_SHMFD)))
		return RTE_VHOST_MSG_RESULT_OK;

	/* The frontend may still not support the inflight feature
	 * although we negotiate the protocol feature.
	 */
	if ((!vq->inflight_split))
		return RTE_VHOST_MSG_RESULT_OK;

	if (!vq->inflight_split->version) {
		vq->inflight_split->version = INFLIGHT_VERSION;
		return RTE_VHOST_MSG_RESULT_OK;
	}

	if (vq->resubmit_inflight)
		return RTE_VHOST_MSG_RESULT_OK;

	inflight_split = vq->inflight_split;
	vq->global_counter = 0;
	last_io = inflight_split->last_inflight_io;

	if (inflight_split->used_idx != used->idx) {
		inflight_split->desc[last_io].inflight = 0;
		rte_atomic_thread_fence(__ATOMIC_SEQ_CST);
		inflight_split->used_idx = used->idx;
	}

	for (i = 0; i < inflight_split->desc_num; i++) {
		if (inflight_split->desc[i].inflight == 1)
			resubmit_num++;
	}

	vq->last_avail_idx += resubmit_num;

	if (resubmit_num) {
		resubmit = rte_zmalloc_socket("resubmit", sizeof(struct rte_vhost_resubmit_info),
				0, vq->numa_node);
		if (!resubmit) {
			VHOST_LOG_CONFIG(ERR,
					"(%s) failed to allocate memory for resubmit info.\n",
					dev->ifname);
			return RTE_VHOST_MSG_RESULT_ERR;
		}

		resubmit->resubmit_list = rte_zmalloc_socket("resubmit_list",
				resubmit_num * sizeof(struct rte_vhost_resubmit_desc),
				0, vq->numa_node);
		if (!resubmit->resubmit_list) {
			VHOST_LOG_CONFIG(ERR,
					"(%s) failed to allocate memory for inflight desc.\n",
					dev->ifname);
			rte_free(resubmit);
			return RTE_VHOST_MSG_RESULT_ERR;
		}

		num = 0;
		for (i = 0; i < vq->inflight_split->desc_num; i++) {
			if (vq->inflight_split->desc[i].inflight == 1) {
				resubmit->resubmit_list[num].index = i;
				resubmit->resubmit_list[num].counter =
					inflight_split->desc[i].counter;
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