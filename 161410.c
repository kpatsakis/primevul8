s64 __ktime_divns(const ktime_t kt, s64 div)
{
	int sft = 0;
	s64 dclc;
	u64 tmp;

	dclc = ktime_to_ns(kt);
	tmp = dclc < 0 ? -dclc : dclc;

	/* Make sure the divisor is less than 2^32: */
	while (div >> 32) {
		sft++;
		div >>= 1;
	}
	tmp >>= sft;
	do_div(tmp, (unsigned long) div);
	return dclc < 0 ? -tmp : tmp;
}