int bnxt_re_query_qp(struct ib_qp *ib_qp, struct ib_qp_attr *qp_attr,
		     int qp_attr_mask, struct ib_qp_init_attr *qp_init_attr)
{
	struct bnxt_re_qp *qp = container_of(ib_qp, struct bnxt_re_qp, ib_qp);
	struct bnxt_re_dev *rdev = qp->rdev;
	struct bnxt_qplib_qp *qplib_qp;
	int rc;

	qplib_qp = kzalloc(sizeof(*qplib_qp), GFP_KERNEL);
	if (!qplib_qp)
		return -ENOMEM;

	qplib_qp->id = qp->qplib_qp.id;
	qplib_qp->ah.host_sgid_index = qp->qplib_qp.ah.host_sgid_index;

	rc = bnxt_qplib_query_qp(&rdev->qplib_res, qplib_qp);
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Failed to query HW QP");
		goto out;
	}
	qp_attr->qp_state = __to_ib_qp_state(qplib_qp->state);
	qp_attr->en_sqd_async_notify = qplib_qp->en_sqd_async_notify ? 1 : 0;
	qp_attr->qp_access_flags = __to_ib_access_flags(qplib_qp->access);
	qp_attr->pkey_index = qplib_qp->pkey_index;
	qp_attr->qkey = qplib_qp->qkey;
	qp_attr->ah_attr.type = RDMA_AH_ATTR_TYPE_ROCE;
	rdma_ah_set_grh(&qp_attr->ah_attr, NULL, qplib_qp->ah.flow_label,
			qplib_qp->ah.host_sgid_index,
			qplib_qp->ah.hop_limit,
			qplib_qp->ah.traffic_class);
	rdma_ah_set_dgid_raw(&qp_attr->ah_attr, qplib_qp->ah.dgid.data);
	rdma_ah_set_sl(&qp_attr->ah_attr, qplib_qp->ah.sl);
	ether_addr_copy(qp_attr->ah_attr.roce.dmac, qplib_qp->ah.dmac);
	qp_attr->path_mtu = __to_ib_mtu(qplib_qp->path_mtu);
	qp_attr->timeout = qplib_qp->timeout;
	qp_attr->retry_cnt = qplib_qp->retry_cnt;
	qp_attr->rnr_retry = qplib_qp->rnr_retry;
	qp_attr->min_rnr_timer = qplib_qp->min_rnr_timer;
	qp_attr->rq_psn = qplib_qp->rq.psn;
	qp_attr->max_rd_atomic = qplib_qp->max_rd_atomic;
	qp_attr->sq_psn = qplib_qp->sq.psn;
	qp_attr->max_dest_rd_atomic = qplib_qp->max_dest_rd_atomic;
	qp_init_attr->sq_sig_type = qplib_qp->sig_type ? IB_SIGNAL_ALL_WR :
							 IB_SIGNAL_REQ_WR;
	qp_attr->dest_qp_num = qplib_qp->dest_qpn;

	qp_attr->cap.max_send_wr = qp->qplib_qp.sq.max_wqe;
	qp_attr->cap.max_send_sge = qp->qplib_qp.sq.max_sge;
	qp_attr->cap.max_recv_wr = qp->qplib_qp.rq.max_wqe;
	qp_attr->cap.max_recv_sge = qp->qplib_qp.rq.max_sge;
	qp_attr->cap.max_inline_data = qp->qplib_qp.max_inline_data;
	qp_init_attr->cap = qp_attr->cap;

out:
	kfree(qplib_qp);
	return rc;
}