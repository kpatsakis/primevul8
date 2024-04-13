static int bnxt_re_post_send_shadow_qp(struct bnxt_re_dev *rdev,
				       struct bnxt_re_qp *qp,
				       const struct ib_send_wr *wr)
{
	struct bnxt_qplib_swqe wqe;
	int rc = 0, payload_sz = 0;
	unsigned long flags;

	spin_lock_irqsave(&qp->sq_lock, flags);
	memset(&wqe, 0, sizeof(wqe));
	while (wr) {
		/* House keeping */
		memset(&wqe, 0, sizeof(wqe));

		/* Common */
		wqe.num_sge = wr->num_sge;
		if (wr->num_sge > qp->qplib_qp.sq.max_sge) {
			dev_err(rdev_to_dev(rdev),
				"Limit exceeded for Send SGEs");
			rc = -EINVAL;
			goto bad;
		}

		payload_sz = bnxt_re_copy_wr_payload(qp->rdev, wr, &wqe);
		if (payload_sz < 0) {
			rc = -EINVAL;
			goto bad;
		}
		wqe.wr_id = wr->wr_id;

		wqe.type = BNXT_QPLIB_SWQE_TYPE_SEND;

		rc = bnxt_re_build_send_wqe(qp, wr, &wqe);
		if (!rc)
			rc = bnxt_qplib_post_send(&qp->qplib_qp, &wqe);
bad:
		if (rc) {
			dev_err(rdev_to_dev(rdev),
				"Post send failed opcode = %#x rc = %d",
				wr->opcode, rc);
			break;
		}
		wr = wr->next;
	}
	bnxt_qplib_post_send_db(&qp->qplib_qp);
	bnxt_ud_qp_hw_stall_workaround(qp);
	spin_unlock_irqrestore(&qp->sq_lock, flags);
	return rc;
}