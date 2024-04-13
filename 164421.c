static int bits_to_user(unsigned long *bits, unsigned int maxbit,
			unsigned int maxlen, void __user *p, int compat)
{
	int len, i;

	if (compat) {
		len = BITS_TO_LONGS_COMPAT(maxbit) * sizeof(compat_long_t);
		if (len > maxlen)
			len = maxlen;

		for (i = 0; i < len / sizeof(compat_long_t); i++)
			if (copy_to_user((compat_long_t __user *) p + i,
					 (compat_long_t *) bits +
						i + 1 - ((i % 2) << 1),
					 sizeof(compat_long_t)))
				return -EFAULT;
	} else {
		len = BITS_TO_LONGS(maxbit) * sizeof(long);
		if (len > maxlen)
			len = maxlen;

		if (copy_to_user(p, bits, len))
			return -EFAULT;
	}

	return len;
}