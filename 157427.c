static int is_ud_qp(struct bnxt_re_qp *qp)
{
	return (qp->qplib_qp.type == CMDQ_CREATE_QP_TYPE_UD ||
		qp->qplib_qp.type == CMDQ_CREATE_QP_TYPE_GSI);
}