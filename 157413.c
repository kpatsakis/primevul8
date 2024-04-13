static int bnxt_re_init_user_srq(struct bnxt_re_dev *rdev,
				 struct bnxt_re_pd *pd,
				 struct bnxt_re_srq *srq,
				 struct ib_udata *udata)
{
	struct bnxt_re_srq_req ureq;
	struct bnxt_qplib_srq *qplib_srq = &srq->qplib_srq;
	struct ib_umem *umem;
	int bytes = 0;
	struct bnxt_re_ucontext *cntx = rdma_udata_to_drv_context(
		udata, struct bnxt_re_ucontext, ib_uctx);

	if (ib_copy_from_udata(&ureq, udata, sizeof(ureq)))
		return -EFAULT;

	bytes = (qplib_srq->max_wqe * BNXT_QPLIB_MAX_RQE_ENTRY_SIZE);
	bytes = PAGE_ALIGN(bytes);
	umem = ib_umem_get(udata, ureq.srqva, bytes, IB_ACCESS_LOCAL_WRITE, 1);
	if (IS_ERR(umem))
		return PTR_ERR(umem);

	srq->umem = umem;
	qplib_srq->sg_info.sglist = umem->sg_head.sgl;
	qplib_srq->sg_info.npages = ib_umem_num_pages(umem);
	qplib_srq->sg_info.nmap = umem->nmap;
	qplib_srq->srq_handle = ureq.srq_handle;
	qplib_srq->dpi = &cntx->dpi;

	return 0;
}