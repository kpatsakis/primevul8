unsigned long __round_jiffies_up(unsigned long j, int cpu)
{
	return round_jiffies_common(j, cpu, true);
}