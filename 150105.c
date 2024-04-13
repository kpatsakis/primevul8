rb_memsearch_wchar(const unsigned char *xs, long m, const unsigned char *ys, long n)
{
    const unsigned char *x = xs, x0 = *xs, *y = ys;
    enum {char_size = 2};

    for (n -= m; n >= 0; n -= char_size, y += char_size) {
	if (x0 == *y && memcmp(x+1, y+1, m-1) == 0)
	    return y - ys;
    }
    return -1;
}