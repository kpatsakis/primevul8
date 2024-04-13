unsigned long round_jiffies_up(unsigned long j)
{
	return round_jiffies_common(j, raw_smp_processor_id(), true);
}