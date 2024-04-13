unsigned long round_jiffies_relative(unsigned long j)
{
	return __round_jiffies_relative(j, raw_smp_processor_id());
}