int bnxt_re_post_recv(struct ib_qp *ib_qp, const struct ib_recv_wr *wr,
		      const struct ib_recv_wr **bad_wr)
{
	struct bnxt_re_qp *qp = container_of(ib_qp, struct bnxt_re_qp, ib_qp);
	struct bnxt_qplib_swqe wqe;
	int rc = 0, payload_sz = 0;
	unsigned long flags;
	u32 count = 0;

	spin_lock_irqsave(&qp->rq_lock, flags);
	while (wr) {
		/* House keeping */
		memset(&wqe, 0, sizeof(wqe));

		/* Common */
		wqe.num_sge = wr->num_sge;
		if (wr->num_sge > qp->qplib_qp.rq.max_sge) {
			dev_err(rdev_to_dev(qp->rdev),
				"Limit exceeded for Receive SGEs");
			rc = -EINVAL;
			*bad_wr = wr;
			break;
		}

		payload_sz = bnxt_re_build_sgl(wr->sg_list, wqe.sg_list,
					       wr->num_sge);
		wqe.wr_id = wr->wr_id;
		wqe.type = BNXT_QPLIB_SWQE_TYPE_RECV;

		if (ib_qp->qp_type == IB_QPT_GSI &&
		    qp->qplib_qp.type != CMDQ_CREATE_QP_TYPE_GSI)
			rc = bnxt_re_build_qp1_shadow_qp_recv(qp, wr, &wqe,
							      payload_sz);
		if (!rc)
			rc = bnxt_qplib_post_recv(&qp->qplib_qp, &wqe);
		if (rc) {
			*bad_wr = wr;
			break;
		}

		/* Ring DB if the RQEs posted reaches a threshold value */
		if (++count >= BNXT_RE_RQ_WQE_THRESHOLD) {
			bnxt_qplib_post_recv_db(&qp->qplib_qp);
			count = 0;
		}

		wr = wr->next;
	}

	if (count)
		bnxt_qplib_post_recv_db(&qp->qplib_qp);

	spin_unlock_irqrestore(&qp->rq_lock, flags);

	return rc;
}