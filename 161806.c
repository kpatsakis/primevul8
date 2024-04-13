unsigned long __round_jiffies(unsigned long j, int cpu)
{
	return round_jiffies_common(j, cpu, false);
}