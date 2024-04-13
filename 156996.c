	__releases(&hb->lock)
{
	__queue_me(q, hb);
	spin_unlock(&hb->lock);
}