static int bnxt_re_init_user_qp(struct bnxt_re_dev *rdev, struct bnxt_re_pd *pd,
				struct bnxt_re_qp *qp, struct ib_udata *udata)
{
	struct bnxt_re_qp_req ureq;
	struct bnxt_qplib_qp *qplib_qp = &qp->qplib_qp;
	struct ib_umem *umem;
	int bytes = 0, psn_sz;
	struct bnxt_re_ucontext *cntx = rdma_udata_to_drv_context(
		udata, struct bnxt_re_ucontext, ib_uctx);

	if (ib_copy_from_udata(&ureq, udata, sizeof(ureq)))
		return -EFAULT;

	bytes = (qplib_qp->sq.max_wqe * BNXT_QPLIB_MAX_SQE_ENTRY_SIZE);
	/* Consider mapping PSN search memory only for RC QPs. */
	if (qplib_qp->type == CMDQ_CREATE_QP_TYPE_RC) {
		psn_sz = bnxt_qplib_is_chip_gen_p5(&rdev->chip_ctx) ?
					sizeof(struct sq_psn_search_ext) :
					sizeof(struct sq_psn_search);
		bytes += (qplib_qp->sq.max_wqe * psn_sz);
	}
	bytes = PAGE_ALIGN(bytes);
	umem = ib_umem_get(udata, ureq.qpsva, bytes, IB_ACCESS_LOCAL_WRITE, 1);
	if (IS_ERR(umem))
		return PTR_ERR(umem);

	qp->sumem = umem;
	qplib_qp->sq.sg_info.sglist = umem->sg_head.sgl;
	qplib_qp->sq.sg_info.npages = ib_umem_num_pages(umem);
	qplib_qp->sq.sg_info.nmap = umem->nmap;
	qplib_qp->qp_handle = ureq.qp_handle;

	if (!qp->qplib_qp.srq) {
		bytes = (qplib_qp->rq.max_wqe * BNXT_QPLIB_MAX_RQE_ENTRY_SIZE);
		bytes = PAGE_ALIGN(bytes);
		umem = ib_umem_get(udata, ureq.qprva, bytes,
				   IB_ACCESS_LOCAL_WRITE, 1);
		if (IS_ERR(umem))
			goto rqfail;
		qp->rumem = umem;
		qplib_qp->rq.sg_info.sglist = umem->sg_head.sgl;
		qplib_qp->rq.sg_info.npages = ib_umem_num_pages(umem);
		qplib_qp->rq.sg_info.nmap = umem->nmap;
	}

	qplib_qp->dpi = &cntx->dpi;
	return 0;
rqfail:
	ib_umem_release(qp->sumem);
	qp->sumem = NULL;
	memset(&qplib_qp->sq.sg_info, 0, sizeof(qplib_qp->sq.sg_info));

	return PTR_ERR(umem);
}