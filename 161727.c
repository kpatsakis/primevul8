bool hrtimer_active(const struct hrtimer *timer)
{
	struct hrtimer_cpu_base *cpu_base;
	unsigned int seq;

	do {
		cpu_base = READ_ONCE(timer->base->cpu_base);
		seq = raw_read_seqcount_begin(&cpu_base->seq);

		if (timer->state != HRTIMER_STATE_INACTIVE ||
		    cpu_base->running == timer)
			return true;

	} while (read_seqcount_retry(&cpu_base->seq, seq) ||
		 cpu_base != READ_ONCE(timer->base->cpu_base));

	return false;
}