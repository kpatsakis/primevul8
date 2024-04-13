void bnxt_re_destroy_srq(struct ib_srq *ib_srq, struct ib_udata *udata)
{
	struct bnxt_re_srq *srq = container_of(ib_srq, struct bnxt_re_srq,
					       ib_srq);
	struct bnxt_re_dev *rdev = srq->rdev;
	struct bnxt_qplib_srq *qplib_srq = &srq->qplib_srq;
	struct bnxt_qplib_nq *nq = NULL;

	if (qplib_srq->cq)
		nq = qplib_srq->cq->nq;
	bnxt_qplib_destroy_srq(&rdev->qplib_res, qplib_srq);
	ib_umem_release(srq->umem);
	atomic_dec(&rdev->srq_count);
	if (nq)
		nq->budget--;
}