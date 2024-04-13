rb_memsearch_ss(const unsigned char *xs, long m, const unsigned char *ys, long n)
{
    const unsigned char *x = xs, *xe = xs + m;
    const unsigned char *y = ys, *ye = ys + n;
#ifndef VALUE_MAX
# if SIZEOF_VALUE == 8
#  define VALUE_MAX 0xFFFFFFFFFFFFFFFFULL
# elif SIZEOF_VALUE == 4
#  define VALUE_MAX 0xFFFFFFFFUL
# endif
#endif
    VALUE hx, hy, mask = VALUE_MAX >> ((SIZEOF_VALUE - m) * CHAR_BIT);

    if (m > SIZEOF_VALUE)
	rb_bug("!!too long pattern string!!");

    if (!(y = memchr(y, *x, n - m + 1)))
	return -1;

    /* Prepare hash value */
    for (hx = *x++, hy = *y++; x < xe; ++x, ++y) {
	hx <<= CHAR_BIT;
	hy <<= CHAR_BIT;
	hx |= *x;
	hy |= *y;
    }
    /* Searching */
    while (hx != hy) {
	if (y == ye)
	    return -1;
	hy <<= CHAR_BIT;
	hy |= *y;
	hy &= mask;
	y++;
    }
    return y - ys - m;
}