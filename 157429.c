static int bnxt_re_post_recv_shadow_qp(struct bnxt_re_dev *rdev,
				       struct bnxt_re_qp *qp,
				       const struct ib_recv_wr *wr)
{
	struct bnxt_qplib_swqe wqe;
	int rc = 0;

	memset(&wqe, 0, sizeof(wqe));
	while (wr) {
		/* House keeping */
		memset(&wqe, 0, sizeof(wqe));

		/* Common */
		wqe.num_sge = wr->num_sge;
		if (wr->num_sge > qp->qplib_qp.rq.max_sge) {
			dev_err(rdev_to_dev(rdev),
				"Limit exceeded for Receive SGEs");
			rc = -EINVAL;
			break;
		}
		bnxt_re_build_sgl(wr->sg_list, wqe.sg_list, wr->num_sge);
		wqe.wr_id = wr->wr_id;
		wqe.type = BNXT_QPLIB_SWQE_TYPE_RECV;

		rc = bnxt_qplib_post_recv(&qp->qplib_qp, &wqe);
		if (rc)
			break;

		wr = wr->next;
	}
	if (!rc)
		bnxt_qplib_post_recv_db(&qp->qplib_qp);
	return rc;
}