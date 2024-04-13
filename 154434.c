static inline int u64_shl_div_u64(u64 a, unsigned int shift,
				  u64 divisor, u64 *result)
{
	u64 low = a << shift, high = a >> (64 - shift);

	/* To avoid the overflow on divq */
	if (high >= divisor)
		return 1;

	/* Low hold the result, high hold rem which is discarded */
	asm("divq %2\n\t" : "=a" (low), "=d" (high) :
	    "rm" (divisor), "0" (low), "1" (high));
	*result = low;

	return 0;
}