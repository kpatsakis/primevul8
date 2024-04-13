unsigned long round_jiffies_up_relative(unsigned long j)
{
	return __round_jiffies_up_relative(j, raw_smp_processor_id());
}