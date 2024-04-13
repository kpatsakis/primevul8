static void bnxt_ud_qp_hw_stall_workaround(struct bnxt_re_qp *qp)
{
	if ((qp->ib_qp.qp_type == IB_QPT_UD ||
	     qp->ib_qp.qp_type == IB_QPT_GSI ||
	     qp->ib_qp.qp_type == IB_QPT_RAW_ETHERTYPE) &&
	     qp->qplib_qp.wqe_cnt == BNXT_RE_UD_QP_HW_STALL) {
		int qp_attr_mask;
		struct ib_qp_attr qp_attr;

		qp_attr_mask = IB_QP_STATE;
		qp_attr.qp_state = IB_QPS_RTS;
		bnxt_re_modify_qp(&qp->ib_qp, &qp_attr, qp_attr_mask, NULL);
		qp->qplib_qp.wqe_cnt = 0;
	}
}