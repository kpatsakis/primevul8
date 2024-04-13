struct ib_qp *bnxt_re_create_qp(struct ib_pd *ib_pd,
				struct ib_qp_init_attr *qp_init_attr,
				struct ib_udata *udata)
{
	struct bnxt_re_pd *pd = container_of(ib_pd, struct bnxt_re_pd, ib_pd);
	struct bnxt_re_dev *rdev = pd->rdev;
	struct bnxt_qplib_dev_attr *dev_attr = &rdev->dev_attr;
	struct bnxt_re_qp *qp;
	struct bnxt_re_cq *cq;
	struct bnxt_re_srq *srq;
	int rc, entries;

	if ((qp_init_attr->cap.max_send_wr > dev_attr->max_qp_wqes) ||
	    (qp_init_attr->cap.max_recv_wr > dev_attr->max_qp_wqes) ||
	    (qp_init_attr->cap.max_send_sge > dev_attr->max_qp_sges) ||
	    (qp_init_attr->cap.max_recv_sge > dev_attr->max_qp_sges) ||
	    (qp_init_attr->cap.max_inline_data > dev_attr->max_inline_data))
		return ERR_PTR(-EINVAL);

	qp = kzalloc(sizeof(*qp), GFP_KERNEL);
	if (!qp)
		return ERR_PTR(-ENOMEM);

	qp->rdev = rdev;
	ether_addr_copy(qp->qplib_qp.smac, rdev->netdev->dev_addr);
	qp->qplib_qp.pd = &pd->qplib_pd;
	qp->qplib_qp.qp_handle = (u64)(unsigned long)(&qp->qplib_qp);
	qp->qplib_qp.type = __from_ib_qp_type(qp_init_attr->qp_type);

	if (qp_init_attr->qp_type == IB_QPT_GSI &&
	    bnxt_qplib_is_chip_gen_p5(&rdev->chip_ctx))
		qp->qplib_qp.type = CMDQ_CREATE_QP_TYPE_GSI;
	if (qp->qplib_qp.type == IB_QPT_MAX) {
		dev_err(rdev_to_dev(rdev), "QP type 0x%x not supported",
			qp->qplib_qp.type);
		rc = -EINVAL;
		goto fail;
	}

	qp->qplib_qp.max_inline_data = qp_init_attr->cap.max_inline_data;
	qp->qplib_qp.sig_type = ((qp_init_attr->sq_sig_type ==
				  IB_SIGNAL_ALL_WR) ? true : false);

	qp->qplib_qp.sq.max_sge = qp_init_attr->cap.max_send_sge;
	if (qp->qplib_qp.sq.max_sge > dev_attr->max_qp_sges)
		qp->qplib_qp.sq.max_sge = dev_attr->max_qp_sges;

	if (qp_init_attr->send_cq) {
		cq = container_of(qp_init_attr->send_cq, struct bnxt_re_cq,
				  ib_cq);
		if (!cq) {
			dev_err(rdev_to_dev(rdev), "Send CQ not found");
			rc = -EINVAL;
			goto fail;
		}
		qp->qplib_qp.scq = &cq->qplib_cq;
		qp->scq = cq;
	}

	if (qp_init_attr->recv_cq) {
		cq = container_of(qp_init_attr->recv_cq, struct bnxt_re_cq,
				  ib_cq);
		if (!cq) {
			dev_err(rdev_to_dev(rdev), "Receive CQ not found");
			rc = -EINVAL;
			goto fail;
		}
		qp->qplib_qp.rcq = &cq->qplib_cq;
		qp->rcq = cq;
	}

	if (qp_init_attr->srq) {
		srq = container_of(qp_init_attr->srq, struct bnxt_re_srq,
				   ib_srq);
		if (!srq) {
			dev_err(rdev_to_dev(rdev), "SRQ not found");
			rc = -EINVAL;
			goto fail;
		}
		qp->qplib_qp.srq = &srq->qplib_srq;
		qp->qplib_qp.rq.max_wqe = 0;
	} else {
		/* Allocate 1 more than what's provided so posting max doesn't
		 * mean empty
		 */
		entries = roundup_pow_of_two(qp_init_attr->cap.max_recv_wr + 1);
		qp->qplib_qp.rq.max_wqe = min_t(u32, entries,
						dev_attr->max_qp_wqes + 1);

		qp->qplib_qp.rq.q_full_delta = qp->qplib_qp.rq.max_wqe -
						qp_init_attr->cap.max_recv_wr;

		qp->qplib_qp.rq.max_sge = qp_init_attr->cap.max_recv_sge;
		if (qp->qplib_qp.rq.max_sge > dev_attr->max_qp_sges)
			qp->qplib_qp.rq.max_sge = dev_attr->max_qp_sges;
	}

	qp->qplib_qp.mtu = ib_mtu_enum_to_int(iboe_get_mtu(rdev->netdev->mtu));

	if (qp_init_attr->qp_type == IB_QPT_GSI &&
	    !(bnxt_qplib_is_chip_gen_p5(&rdev->chip_ctx))) {
		/* Allocate 1 more than what's provided */
		entries = roundup_pow_of_two(qp_init_attr->cap.max_send_wr + 1);
		qp->qplib_qp.sq.max_wqe = min_t(u32, entries,
						dev_attr->max_qp_wqes + 1);
		qp->qplib_qp.sq.q_full_delta = qp->qplib_qp.sq.max_wqe -
						qp_init_attr->cap.max_send_wr;
		qp->qplib_qp.rq.max_sge = dev_attr->max_qp_sges;
		if (qp->qplib_qp.rq.max_sge > dev_attr->max_qp_sges)
			qp->qplib_qp.rq.max_sge = dev_attr->max_qp_sges;
		qp->qplib_qp.sq.max_sge++;
		if (qp->qplib_qp.sq.max_sge > dev_attr->max_qp_sges)
			qp->qplib_qp.sq.max_sge = dev_attr->max_qp_sges;

		qp->qplib_qp.rq_hdr_buf_size =
					BNXT_QPLIB_MAX_QP1_RQ_HDR_SIZE_V2;

		qp->qplib_qp.sq_hdr_buf_size =
					BNXT_QPLIB_MAX_QP1_SQ_HDR_SIZE_V2;
		qp->qplib_qp.dpi = &rdev->dpi_privileged;
		rc = bnxt_qplib_create_qp1(&rdev->qplib_res, &qp->qplib_qp);
		if (rc) {
			dev_err(rdev_to_dev(rdev), "Failed to create HW QP1");
			goto fail;
		}
		/* Create a shadow QP to handle the QP1 traffic */
		rdev->qp1_sqp = bnxt_re_create_shadow_qp(pd, &rdev->qplib_res,
							 &qp->qplib_qp);
		if (!rdev->qp1_sqp) {
			rc = -EINVAL;
			dev_err(rdev_to_dev(rdev),
				"Failed to create Shadow QP for QP1");
			goto qp_destroy;
		}
		rdev->sqp_ah = bnxt_re_create_shadow_qp_ah(pd, &rdev->qplib_res,
							   &qp->qplib_qp);
		if (!rdev->sqp_ah) {
			bnxt_qplib_destroy_qp(&rdev->qplib_res,
					      &rdev->qp1_sqp->qplib_qp);
			rc = -EINVAL;
			dev_err(rdev_to_dev(rdev),
				"Failed to create AH entry for ShadowQP");
			goto qp_destroy;
		}

	} else {
		/* Allocate 128 + 1 more than what's provided */
		entries = roundup_pow_of_two(qp_init_attr->cap.max_send_wr +
					     BNXT_QPLIB_RESERVED_QP_WRS + 1);
		qp->qplib_qp.sq.max_wqe = min_t(u32, entries,
						dev_attr->max_qp_wqes +
						BNXT_QPLIB_RESERVED_QP_WRS + 1);
		qp->qplib_qp.sq.q_full_delta = BNXT_QPLIB_RESERVED_QP_WRS + 1;

		/*
		 * Reserving one slot for Phantom WQE. Application can
		 * post one extra entry in this case. But allowing this to avoid
		 * unexpected Queue full condition
		 */

		qp->qplib_qp.sq.q_full_delta -= 1;

		qp->qplib_qp.max_rd_atomic = dev_attr->max_qp_rd_atom;
		qp->qplib_qp.max_dest_rd_atomic = dev_attr->max_qp_init_rd_atom;
		if (udata) {
			rc = bnxt_re_init_user_qp(rdev, pd, qp, udata);
			if (rc)
				goto fail;
		} else {
			qp->qplib_qp.dpi = &rdev->dpi_privileged;
		}

		rc = bnxt_qplib_create_qp(&rdev->qplib_res, &qp->qplib_qp);
		if (rc) {
			dev_err(rdev_to_dev(rdev), "Failed to create HW QP");
			goto free_umem;
		}
	}

	qp->ib_qp.qp_num = qp->qplib_qp.id;
	spin_lock_init(&qp->sq_lock);
	spin_lock_init(&qp->rq_lock);

	if (udata) {
		struct bnxt_re_qp_resp resp;

		resp.qpid = qp->ib_qp.qp_num;
		resp.rsvd = 0;
		rc = ib_copy_to_udata(udata, &resp, sizeof(resp));
		if (rc) {
			dev_err(rdev_to_dev(rdev), "Failed to copy QP udata");
			goto qp_destroy;
		}
	}
	INIT_LIST_HEAD(&qp->list);
	mutex_lock(&rdev->qp_lock);
	list_add_tail(&qp->list, &rdev->qp_list);
	atomic_inc(&rdev->qp_count);
	mutex_unlock(&rdev->qp_lock);

	return &qp->ib_qp;
qp_destroy:
	bnxt_qplib_destroy_qp(&rdev->qplib_res, &qp->qplib_qp);
free_umem:
	ib_umem_release(qp->rumem);
	ib_umem_release(qp->sumem);
fail:
	kfree(qp);
	return ERR_PTR(rc);
}