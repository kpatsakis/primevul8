int bnxt_re_modify_srq(struct ib_srq *ib_srq, struct ib_srq_attr *srq_attr,
		       enum ib_srq_attr_mask srq_attr_mask,
		       struct ib_udata *udata)
{
	struct bnxt_re_srq *srq = container_of(ib_srq, struct bnxt_re_srq,
					       ib_srq);
	struct bnxt_re_dev *rdev = srq->rdev;
	int rc;

	switch (srq_attr_mask) {
	case IB_SRQ_MAX_WR:
		/* SRQ resize is not supported */
		break;
	case IB_SRQ_LIMIT:
		/* Change the SRQ threshold */
		if (srq_attr->srq_limit > srq->qplib_srq.max_wqe)
			return -EINVAL;

		srq->qplib_srq.threshold = srq_attr->srq_limit;
		rc = bnxt_qplib_modify_srq(&rdev->qplib_res, &srq->qplib_srq);
		if (rc) {
			dev_err(rdev_to_dev(rdev), "Modify HW SRQ failed!");
			return rc;
		}
		/* On success, update the shadow */
		srq->srq_limit = srq_attr->srq_limit;
		/* No need to Build and send response back to udata */
		break;
	default:
		dev_err(rdev_to_dev(rdev),
			"Unsupported srq_attr_mask 0x%x", srq_attr_mask);
		return -EINVAL;
	}
	return 0;
}