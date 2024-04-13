	__acquires(&hb->lock)
{
	struct futex_hash_bucket *hb;

	hb = hash_futex(&q->key);

	/*
	 * Increment the counter before taking the lock so that
	 * a potential waker won't miss a to-be-slept task that is
	 * waiting for the spinlock. This is safe as all queue_lock()
	 * users end up calling queue_me(). Similarly, for housekeeping,
	 * decrement the counter at queue_unlock() when some error has
	 * occurred and we don't end up adding the task to the list.
	 */
	hb_waiters_inc(hb);

	q->lock_ptr = &hb->lock;

	spin_lock(&hb->lock); /* implies smp_mb(); (A) */
	return hb;
}