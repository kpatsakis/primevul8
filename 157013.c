static inline int hb_waiters_pending(struct futex_hash_bucket *hb)
{
#ifdef CONFIG_SMP
	return atomic_read(&hb->waiters);
#else
	return 1;
#endif
}