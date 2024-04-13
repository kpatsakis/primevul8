	__releases(&hb->lock)
{
	spin_unlock(&hb->lock);
	hb_waiters_dec(hb);
}