static inline void futex_get_mm(union futex_key *key)
{
	mmgrab(key->private.mm);
	/*
	 * Ensure futex_get_mm() implies a full barrier such that
	 * get_futex_key() implies a full barrier. This is relied upon
	 * as smp_mb(); (B), see the ordering comment above.
	 */
	smp_mb__after_atomic();
}