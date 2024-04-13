unsigned long __round_jiffies_up_relative(unsigned long j, int cpu)
{
	unsigned long j0 = jiffies;

	/* Use j0 because jiffies might change while we run */
	return round_jiffies_common(j + j0, cpu, true) - j0;
}