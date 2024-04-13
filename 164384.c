static int bits_to_user(unsigned long *bits, unsigned int maxbit,
			unsigned int maxlen, void __user *p, int compat)
{
	int len = compat ?
			BITS_TO_LONGS_COMPAT(maxbit) * sizeof(compat_long_t) :
			BITS_TO_LONGS(maxbit) * sizeof(long);

	if (len > maxlen)
		len = maxlen;

	return copy_to_user(p, bits, len) ? -EFAULT : len;
}