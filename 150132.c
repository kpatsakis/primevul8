rb_memsearch_ss(const unsigned char *xs, long m, const unsigned char *ys, long n)
{
    const unsigned char *y;

    if (y = memmem(ys, n, xs, m))
	return y - ys;
    else
	return -1;
}