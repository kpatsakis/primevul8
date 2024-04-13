int bnxt_re_query_srq(struct ib_srq *ib_srq, struct ib_srq_attr *srq_attr)
{
	struct bnxt_re_srq *srq = container_of(ib_srq, struct bnxt_re_srq,
					       ib_srq);
	struct bnxt_re_srq tsrq;
	struct bnxt_re_dev *rdev = srq->rdev;
	int rc;

	/* Get live SRQ attr */
	tsrq.qplib_srq.id = srq->qplib_srq.id;
	rc = bnxt_qplib_query_srq(&rdev->qplib_res, &tsrq.qplib_srq);
	if (rc) {
		dev_err(rdev_to_dev(rdev), "Query HW SRQ failed!");
		return rc;
	}
	srq_attr->max_wr = srq->qplib_srq.max_wqe;
	srq_attr->max_sge = srq->qplib_srq.max_sge;
	srq_attr->srq_limit = tsrq.qplib_srq.threshold;

	return 0;
}