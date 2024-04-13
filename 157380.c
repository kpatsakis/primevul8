	__acquires(&qp->scq->cq_lock) __acquires(&qp->rcq->cq_lock)
{
	unsigned long flags;

	spin_lock_irqsave(&qp->scq->cq_lock, flags);
	if (qp->rcq != qp->scq)
		spin_lock(&qp->rcq->cq_lock);
	else
		__acquire(&qp->rcq->cq_lock);

	return flags;
}