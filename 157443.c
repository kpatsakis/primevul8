static int send_phantom_wqe(struct bnxt_re_qp *qp)
{
	struct bnxt_qplib_qp *lib_qp = &qp->qplib_qp;
	unsigned long flags;
	int rc = 0;

	spin_lock_irqsave(&qp->sq_lock, flags);

	rc = bnxt_re_bind_fence_mw(lib_qp);
	if (!rc) {
		lib_qp->sq.phantom_wqe_cnt++;
		dev_dbg(&lib_qp->sq.hwq.pdev->dev,
			"qp %#x sq->prod %#x sw_prod %#x phantom_wqe_cnt %d\n",
			lib_qp->id, lib_qp->sq.hwq.prod,
			HWQ_CMP(lib_qp->sq.hwq.prod, &lib_qp->sq.hwq),
			lib_qp->sq.phantom_wqe_cnt);
	}

	spin_unlock_irqrestore(&qp->sq_lock, flags);
	return rc;
}