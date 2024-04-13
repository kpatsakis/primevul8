unsigned long round_jiffies(unsigned long j)
{
	return round_jiffies_common(j, raw_smp_processor_id(), false);
}