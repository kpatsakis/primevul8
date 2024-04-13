int bnxt_re_post_srq_recv(struct ib_srq *ib_srq, const struct ib_recv_wr *wr,
			  const struct ib_recv_wr **bad_wr)
{
	struct bnxt_re_srq *srq = container_of(ib_srq, struct bnxt_re_srq,
					       ib_srq);
	struct bnxt_qplib_swqe wqe;
	unsigned long flags;
	int rc = 0;

	spin_lock_irqsave(&srq->lock, flags);
	while (wr) {
		/* Transcribe each ib_recv_wr to qplib_swqe */
		wqe.num_sge = wr->num_sge;
		bnxt_re_build_sgl(wr->sg_list, wqe.sg_list, wr->num_sge);
		wqe.wr_id = wr->wr_id;
		wqe.type = BNXT_QPLIB_SWQE_TYPE_RECV;

		rc = bnxt_qplib_post_srq_recv(&srq->qplib_srq, &wqe);
		if (rc) {
			*bad_wr = wr;
			break;
		}
		wr = wr->next;
	}
	spin_unlock_irqrestore(&srq->lock, flags);

	return rc;
}