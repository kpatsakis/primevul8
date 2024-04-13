void bnxt_re_destroy_cq(struct ib_cq *ib_cq, struct ib_udata *udata)
{
	struct bnxt_re_cq *cq;
	struct bnxt_qplib_nq *nq;
	struct bnxt_re_dev *rdev;

	cq = container_of(ib_cq, struct bnxt_re_cq, ib_cq);
	rdev = cq->rdev;
	nq = cq->qplib_cq.nq;

	bnxt_qplib_destroy_cq(&rdev->qplib_res, &cq->qplib_cq);
	ib_umem_release(cq->umem);

	atomic_dec(&rdev->cq_count);
	nq->budget--;
	kfree(cq->cql);
}