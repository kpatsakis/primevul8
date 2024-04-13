static u32 wqattrs_hash(const struct workqueue_attrs *attrs)
{
	u32 hash = 0;

	hash = jhash_1word(attrs->nice, hash);
	hash = jhash(cpumask_bits(attrs->cpumask),
		     BITS_TO_LONGS(nr_cpumask_bits) * sizeof(long), hash);
	return hash;
}