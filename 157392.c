static int bnxt_re_modify_shadow_qp(struct bnxt_re_dev *rdev,
				    struct bnxt_re_qp *qp1_qp,
				    int qp_attr_mask)
{
	struct bnxt_re_qp *qp = rdev->qp1_sqp;
	int rc = 0;

	if (qp_attr_mask & IB_QP_STATE) {
		qp->qplib_qp.modify_flags |= CMDQ_MODIFY_QP_MODIFY_MASK_STATE;
		qp->qplib_qp.state = qp1_qp->qplib_qp.state;
	}
	if (qp_attr_mask & IB_QP_PKEY_INDEX) {
		qp->qplib_qp.modify_flags |= CMDQ_MODIFY_QP_MODIFY_MASK_PKEY;
		qp->qplib_qp.pkey_index = qp1_qp->qplib_qp.pkey_index;
	}

	if (qp_attr_mask & IB_QP_QKEY) {
		qp->qplib_qp.modify_flags |= CMDQ_MODIFY_QP_MODIFY_MASK_QKEY;
		/* Using a Random  QKEY */
		qp->qplib_qp.qkey = 0x81818181;
	}
	if (qp_attr_mask & IB_QP_SQ_PSN) {
		qp->qplib_qp.modify_flags |= CMDQ_MODIFY_QP_MODIFY_MASK_SQ_PSN;
		qp->qplib_qp.sq.psn = qp1_qp->qplib_qp.sq.psn;
	}

	rc = bnxt_qplib_modify_qp(&rdev->qplib_res, &qp->qplib_qp);
	if (rc)
		dev_err(rdev_to_dev(rdev),
			"Failed to modify Shadow QP for QP1");
	return rc;
}