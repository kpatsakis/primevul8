int bnxt_re_modify_qp(struct ib_qp *ib_qp, struct ib_qp_attr *qp_attr,
		      int qp_attr_mask, struct ib_udata *udata)
{
	struct bnxt_re_qp *qp = container_of(ib_qp, struct bnxt_re_qp, ib_qp);
	struct bnxt_re_dev *rdev = qp->rdev;
	struct bnxt_qplib_dev_attr *dev_attr = &rdev->dev_attr;
	enum ib_qp_state curr_qp_state, new_qp_state;
	int rc, entries;
	unsigned int flags;
	u8 nw_type;

	qp->qplib_qp.modify_flags = 0;
	if (qp_attr_mask & IB_QP_STATE) {
		curr_qp_state = __to_ib_qp_state(qp->qplib_qp.cur_qp_state);
		new_qp_state = qp_attr->qp_state;
		if (!ib_modify_qp_is_ok(curr_qp_state, new_qp_state,
					ib_qp->qp_type, qp_attr_mask)) {
			dev_err(rdev_to_dev(rdev),
				"Invalid attribute mask: %#x specified ",
				qp_attr_mask);
			dev_err(rdev_to_dev(rdev),
				"for qpn: %#x type: %#x",
				ib_qp->qp_num, ib_qp->qp_type);
			dev_err(rdev_to_dev(rdev),
				"curr_qp_state=0x%x, new_qp_state=0x%x\n",
				curr_qp_state, new_qp_state);
			return -EINVAL;
		}
		qp->qplib_qp.modify_flags |= CMDQ_MODIFY_QP_MODIFY_MASK_STATE;
		qp->qplib_qp.state = __from_ib_qp_state(qp_attr->qp_state);

		if (!qp->sumem &&
		    qp->qplib_qp.state == CMDQ_MODIFY_QP_NEW_STATE_ERR) {
			dev_dbg(rdev_to_dev(rdev),
				"Move QP = %p to flush list\n",
				qp);
			flags = bnxt_re_lock_cqs(qp);
			bnxt_qplib_add_flush_qp(&qp->qplib_qp);
			bnxt_re_unlock_cqs(qp, flags);
		}
		if (!qp->sumem &&
		    qp->qplib_qp.state == CMDQ_MODIFY_QP_NEW_STATE_RESET) {
			dev_dbg(rdev_to_dev(rdev),
				"Move QP = %p out of flush list\n",
				qp);
			flags = bnxt_re_lock_cqs(qp);
			bnxt_qplib_clean_qp(&qp->qplib_qp);
			bnxt_re_unlock_cqs(qp, flags);
		}
	}
	if (qp_attr_mask & IB_QP_EN_SQD_ASYNC_NOTIFY) {
		qp->qplib_qp.modify_flags |=
				CMDQ_MODIFY_QP_MODIFY_MASK_EN_SQD_ASYNC_NOTIFY;
		qp->qplib_qp.en_sqd_async_notify = true;
	}
	if (qp_attr_mask & IB_QP_ACCESS_FLAGS) {
		qp->qplib_qp.modify_flags |= CMDQ_MODIFY_QP_MODIFY_MASK_ACCESS;
		qp->qplib_qp.access =
			__from_ib_access_flags(qp_attr->qp_access_flags);
		/* LOCAL_WRITE access must be set to allow RC receive */
		qp->qplib_qp.access |= BNXT_QPLIB_ACCESS_LOCAL_WRITE;
		/* Temp: Set all params on QP as of now */
		qp->qplib_qp.access |= CMDQ_MODIFY_QP_ACCESS_REMOTE_WRITE;
		qp->qplib_qp.access |= CMDQ_MODIFY_QP_ACCESS_REMOTE_READ;
	}
	if (qp_attr_mask & IB_QP_PKEY_INDEX) {
		qp->qplib_qp.modify_flags |= CMDQ_MODIFY_QP_MODIFY_MASK_PKEY;
		qp->qplib_qp.pkey_index = qp_attr->pkey_index;
	}
	if (qp_attr_mask & IB_QP_QKEY) {
		qp->qplib_qp.modify_flags |= CMDQ_MODIFY_QP_MODIFY_MASK_QKEY;
		qp->qplib_qp.qkey = qp_attr->qkey;
	}
	if (qp_attr_mask & IB_QP_AV) {
		const struct ib_global_route *grh =
			rdma_ah_read_grh(&qp_attr->ah_attr);
		const struct ib_gid_attr *sgid_attr;

		qp->qplib_qp.modify_flags |= CMDQ_MODIFY_QP_MODIFY_MASK_DGID |
				     CMDQ_MODIFY_QP_MODIFY_MASK_FLOW_LABEL |
				     CMDQ_MODIFY_QP_MODIFY_MASK_SGID_INDEX |
				     CMDQ_MODIFY_QP_MODIFY_MASK_HOP_LIMIT |
				     CMDQ_MODIFY_QP_MODIFY_MASK_TRAFFIC_CLASS |
				     CMDQ_MODIFY_QP_MODIFY_MASK_DEST_MAC |
				     CMDQ_MODIFY_QP_MODIFY_MASK_VLAN_ID;
		memcpy(qp->qplib_qp.ah.dgid.data, grh->dgid.raw,
		       sizeof(qp->qplib_qp.ah.dgid.data));
		qp->qplib_qp.ah.flow_label = grh->flow_label;
		/* If RoCE V2 is enabled, stack will have two entries for
		 * each GID entry. Avoiding this duplicte entry in HW. Dividing
		 * the GID index by 2 for RoCE V2
		 */
		qp->qplib_qp.ah.sgid_index = grh->sgid_index / 2;
		qp->qplib_qp.ah.host_sgid_index = grh->sgid_index;
		qp->qplib_qp.ah.hop_limit = grh->hop_limit;
		qp->qplib_qp.ah.traffic_class = grh->traffic_class;
		qp->qplib_qp.ah.sl = rdma_ah_get_sl(&qp_attr->ah_attr);
		ether_addr_copy(qp->qplib_qp.ah.dmac,
				qp_attr->ah_attr.roce.dmac);

		sgid_attr = qp_attr->ah_attr.grh.sgid_attr;
		rc = rdma_read_gid_l2_fields(sgid_attr, NULL,
					     &qp->qplib_qp.smac[0]);
		if (rc)
			return rc;

		nw_type = rdma_gid_attr_network_type(sgid_attr);
		switch (nw_type) {
		case RDMA_NETWORK_IPV4:
			qp->qplib_qp.nw_type =
				CMDQ_MODIFY_QP_NETWORK_TYPE_ROCEV2_IPV4;
			break;
		case RDMA_NETWORK_IPV6:
			qp->qplib_qp.nw_type =
				CMDQ_MODIFY_QP_NETWORK_TYPE_ROCEV2_IPV6;
			break;
		default:
			qp->qplib_qp.nw_type =
				CMDQ_MODIFY_QP_NETWORK_TYPE_ROCEV1;
			break;
		}
	}

	if (qp_attr_mask & IB_QP_PATH_MTU) {
		qp->qplib_qp.modify_flags |=
				CMDQ_MODIFY_QP_MODIFY_MASK_PATH_MTU;
		qp->qplib_qp.path_mtu = __from_ib_mtu(qp_attr->path_mtu);
		qp->qplib_qp.mtu = ib_mtu_enum_to_int(qp_attr->path_mtu);
	} else if (qp_attr->qp_state == IB_QPS_RTR) {
		qp->qplib_qp.modify_flags |=
			CMDQ_MODIFY_QP_MODIFY_MASK_PATH_MTU;
		qp->qplib_qp.path_mtu =
			__from_ib_mtu(iboe_get_mtu(rdev->netdev->mtu));
		qp->qplib_qp.mtu =
			ib_mtu_enum_to_int(iboe_get_mtu(rdev->netdev->mtu));
	}

	if (qp_attr_mask & IB_QP_TIMEOUT) {
		qp->qplib_qp.modify_flags |= CMDQ_MODIFY_QP_MODIFY_MASK_TIMEOUT;
		qp->qplib_qp.timeout = qp_attr->timeout;
	}
	if (qp_attr_mask & IB_QP_RETRY_CNT) {
		qp->qplib_qp.modify_flags |=
				CMDQ_MODIFY_QP_MODIFY_MASK_RETRY_CNT;
		qp->qplib_qp.retry_cnt = qp_attr->retry_cnt;
	}
	if (qp_attr_mask & IB_QP_RNR_RETRY) {
		qp->qplib_qp.modify_flags |=
				CMDQ_MODIFY_QP_MODIFY_MASK_RNR_RETRY;
		qp->qplib_qp.rnr_retry = qp_attr->rnr_retry;
	}
	if (qp_attr_mask & IB_QP_MIN_RNR_TIMER) {
		qp->qplib_qp.modify_flags |=
				CMDQ_MODIFY_QP_MODIFY_MASK_MIN_RNR_TIMER;
		qp->qplib_qp.min_rnr_timer = qp_attr->min_rnr_timer;
	}
	if (qp_attr_mask & IB_QP_RQ_PSN) {
		qp->qplib_qp.modify_flags |= CMDQ_MODIFY_QP_MODIFY_MASK_RQ_PSN;
		qp->qplib_qp.rq.psn = qp_attr->rq_psn;
	}
	if (qp_attr_mask & IB_QP_MAX_QP_RD_ATOMIC) {
		qp->qplib_qp.modify_flags |=
				CMDQ_MODIFY_QP_MODIFY_MASK_MAX_RD_ATOMIC;
		/* Cap the max_rd_atomic to device max */
		qp->qplib_qp.max_rd_atomic = min_t(u32, qp_attr->max_rd_atomic,
						   dev_attr->max_qp_rd_atom);
	}
	if (qp_attr_mask & IB_QP_SQ_PSN) {
		qp->qplib_qp.modify_flags |= CMDQ_MODIFY_QP_MODIFY_MASK_SQ_PSN;
		qp->qplib_qp.sq.psn = qp_attr->sq_psn;
	}
	if (qp_attr_mask & IB_QP_MAX_DEST_RD_ATOMIC) {
		if (qp_attr->max_dest_rd_atomic >
		    dev_attr->max_qp_init_rd_atom) {
			dev_err(rdev_to_dev(rdev),
				"max_dest_rd_atomic requested%d is > dev_max%d",
				qp_attr->max_dest_rd_atomic,
				dev_attr->max_qp_init_rd_atom);
			return -EINVAL;
		}

		qp->qplib_qp.modify_flags |=
				CMDQ_MODIFY_QP_MODIFY_MASK_MAX_DEST_RD_ATOMIC;
		qp->qplib_qp.max_dest_rd_atomic = qp_attr->max_dest_rd_atomic;
	}
	if (qp_attr_mask & IB_QP_CAP) {
		qp->qplib_qp.modify_flags |=
				CMDQ_MODIFY_QP_MODIFY_MASK_SQ_SIZE |
				CMDQ_MODIFY_QP_MODIFY_MASK_RQ_SIZE |
				CMDQ_MODIFY_QP_MODIFY_MASK_SQ_SGE |
				CMDQ_MODIFY_QP_MODIFY_MASK_RQ_SGE |
				CMDQ_MODIFY_QP_MODIFY_MASK_MAX_INLINE_DATA;
		if ((qp_attr->cap.max_send_wr >= dev_attr->max_qp_wqes) ||
		    (qp_attr->cap.max_recv_wr >= dev_attr->max_qp_wqes) ||
		    (qp_attr->cap.max_send_sge >= dev_attr->max_qp_sges) ||
		    (qp_attr->cap.max_recv_sge >= dev_attr->max_qp_sges) ||
		    (qp_attr->cap.max_inline_data >=
						dev_attr->max_inline_data)) {
			dev_err(rdev_to_dev(rdev),
				"Create QP failed - max exceeded");
			return -EINVAL;
		}
		entries = roundup_pow_of_two(qp_attr->cap.max_send_wr);
		qp->qplib_qp.sq.max_wqe = min_t(u32, entries,
						dev_attr->max_qp_wqes + 1);
		qp->qplib_qp.sq.q_full_delta = qp->qplib_qp.sq.max_wqe -
						qp_attr->cap.max_send_wr;
		/*
		 * Reserving one slot for Phantom WQE. Some application can
		 * post one extra entry in this case. Allowing this to avoid
		 * unexpected Queue full condition
		 */
		qp->qplib_qp.sq.q_full_delta -= 1;
		qp->qplib_qp.sq.max_sge = qp_attr->cap.max_send_sge;
		if (qp->qplib_qp.rq.max_wqe) {
			entries = roundup_pow_of_two(qp_attr->cap.max_recv_wr);
			qp->qplib_qp.rq.max_wqe =
				min_t(u32, entries, dev_attr->max_qp_wqes + 1);
			qp->qplib_qp.rq.q_full_delta = qp->qplib_qp.rq.max_wqe -
						       qp_attr->cap.max_recv_wr;
			qp->qplib_qp.rq.max_sge = qp_attr->cap.max_recv_sge;
		} else {
			/* SRQ was used prior, just ignore the RQ caps */
		}
	}
	if (qp_attr_mask & IB_QP_DEST_QPN) {
		qp->qplib_qp.modify_flags |=
				CMDQ_MODIFY_QP_MODIFY_MASK_DEST_QP_ID;
		qp->qplib_qp.dest_qpn = qp_attr->dest_qp_num;
	}
	rc = bnxt_qplib_modify_qp(&rdev->qplib_res, &qp->qplib_qp);
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Failed to modify HW QP");
		return rc;
	}
	if (ib_qp->qp_type == IB_QPT_GSI && rdev->qp1_sqp)
		rc = bnxt_re_modify_shadow_qp(rdev, qp, qp_attr_mask);
	return rc;
}