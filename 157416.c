static struct bnxt_re_qp *bnxt_re_create_shadow_qp
				(struct bnxt_re_pd *pd,
				 struct bnxt_qplib_res *qp1_res,
				 struct bnxt_qplib_qp *qp1_qp)
{
	struct bnxt_re_dev *rdev = pd->rdev;
	struct bnxt_re_qp *qp;
	int rc;

	qp = kzalloc(sizeof(*qp), GFP_KERNEL);
	if (!qp)
		return NULL;

	qp->rdev = rdev;

	/* Initialize the shadow QP structure from the QP1 values */
	ether_addr_copy(qp->qplib_qp.smac, rdev->netdev->dev_addr);

	qp->qplib_qp.pd = &pd->qplib_pd;
	qp->qplib_qp.qp_handle = (u64)(unsigned long)(&qp->qplib_qp);
	qp->qplib_qp.type = IB_QPT_UD;

	qp->qplib_qp.max_inline_data = 0;
	qp->qplib_qp.sig_type = true;

	/* Shadow QP SQ depth should be same as QP1 RQ depth */
	qp->qplib_qp.sq.max_wqe = qp1_qp->rq.max_wqe;
	qp->qplib_qp.sq.max_sge = 2;
	/* Q full delta can be 1 since it is internal QP */
	qp->qplib_qp.sq.q_full_delta = 1;

	qp->qplib_qp.scq = qp1_qp->scq;
	qp->qplib_qp.rcq = qp1_qp->rcq;

	qp->qplib_qp.rq.max_wqe = qp1_qp->rq.max_wqe;
	qp->qplib_qp.rq.max_sge = qp1_qp->rq.max_sge;
	/* Q full delta can be 1 since it is internal QP */
	qp->qplib_qp.rq.q_full_delta = 1;

	qp->qplib_qp.mtu = qp1_qp->mtu;

	qp->qplib_qp.sq_hdr_buf_size = 0;
	qp->qplib_qp.rq_hdr_buf_size = BNXT_QPLIB_MAX_GRH_HDR_SIZE_IPV6;
	qp->qplib_qp.dpi = &rdev->dpi_privileged;

	rc = bnxt_qplib_create_qp(qp1_res, &qp->qplib_qp);
	if (rc)
		goto fail;

	rdev->sqp_id = qp->qplib_qp.id;

	spin_lock_init(&qp->sq_lock);
	INIT_LIST_HEAD(&qp->list);
	mutex_lock(&rdev->qp_lock);
	list_add_tail(&qp->list, &rdev->qp_list);
	atomic_inc(&rdev->qp_count);
	mutex_unlock(&rdev->qp_lock);
	return qp;
fail:
	kfree(qp);
	return NULL;
}