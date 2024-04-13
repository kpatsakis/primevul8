static void set_work_pool_and_clear_pending(struct work_struct *work,
					    int pool_id)
{
	/*
	 * The following wmb is paired with the implied mb in
	 * test_and_set_bit(PENDING) and ensures all updates to @work made
	 * here are visible to and precede any updates by the next PENDING
	 * owner.
	 */
	smp_wmb();
	set_work_data(work, (unsigned long)pool_id << WORK_OFFQ_POOL_SHIFT, 0);
	/*
	 * The following mb guarantees that previous clear of a PENDING bit
	 * will not be reordered with any speculative LOADS or STORES from
	 * work->current_func, which is executed afterwards.  This possible
	 * reordering can lead to a missed execution on attempt to qeueue
	 * the same @work.  E.g. consider this case:
	 *
	 *   CPU#0                         CPU#1
	 *   ----------------------------  --------------------------------
	 *
	 * 1  STORE event_indicated
	 * 2  queue_work_on() {
	 * 3    test_and_set_bit(PENDING)
	 * 4 }                             set_..._and_clear_pending() {
	 * 5                                 set_work_data() # clear bit
	 * 6                                 smp_mb()
	 * 7                               work->current_func() {
	 * 8				      LOAD event_indicated
	 *				   }
	 *
	 * Without an explicit full barrier speculative LOAD on line 8 can
	 * be executed before CPU#0 does STORE on line 1.  If that happens,
	 * CPU#0 observes the PENDING bit is still set and new execution of
	 * a @work is not queued in a hope, that CPU#1 will eventually
	 * finish the queued @work.  Meanwhile CPU#1 does not see
	 * event_indicated is set, because speculative LOAD was executed
	 * before actual STORE.
	 */
	smp_mb();
}