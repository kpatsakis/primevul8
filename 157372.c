int bnxt_re_req_notify_cq(struct ib_cq *ib_cq,
			  enum ib_cq_notify_flags ib_cqn_flags)
{
	struct bnxt_re_cq *cq = container_of(ib_cq, struct bnxt_re_cq, ib_cq);
	int type = 0, rc = 0;
	unsigned long flags;

	spin_lock_irqsave(&cq->cq_lock, flags);
	/* Trigger on the very next completion */
	if (ib_cqn_flags & IB_CQ_NEXT_COMP)
		type = DBC_DBC_TYPE_CQ_ARMALL;
	/* Trigger on the next solicited completion */
	else if (ib_cqn_flags & IB_CQ_SOLICITED)
		type = DBC_DBC_TYPE_CQ_ARMSE;

	/* Poll to see if there are missed events */
	if ((ib_cqn_flags & IB_CQ_REPORT_MISSED_EVENTS) &&
	    !(bnxt_qplib_is_cq_empty(&cq->qplib_cq))) {
		rc = 1;
		goto exit;
	}
	bnxt_qplib_req_notify_cq(&cq->qplib_cq, type);

exit:
	spin_unlock_irqrestore(&cq->cq_lock, flags);
	return rc;
}