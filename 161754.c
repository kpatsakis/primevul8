void timer_clear_idle(void)
{
	struct timer_base *base = this_cpu_ptr(&timer_bases[BASE_STD]);

	/*
	 * We do this unlocked. The worst outcome is a remote enqueue sending
	 * a pointless IPI, but taking the lock would just make the window for
	 * sending the IPI a few instructions smaller for the cost of taking
	 * the lock in the exit from idle path.
	 */
	base->is_idle = false;
}