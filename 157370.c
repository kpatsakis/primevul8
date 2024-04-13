	__releases(&qp->scq->cq_lock) __releases(&qp->rcq->cq_lock)
{
	if (qp->rcq != qp->scq)
		spin_unlock(&qp->rcq->cq_lock);
	else
		__release(&qp->rcq->cq_lock);
	spin_unlock_irqrestore(&qp->scq->cq_lock, flags);
}