get_random_bits( size_t nbits, int level, int secure )
{
    byte *buf, *p;
    size_t nbytes = (nbits+7)/8;

    if( quick_test && level > 1 )
	level = 1;
    MASK_LEVEL(level);
    if( level == 1 ) {
	rndstats.getbytes1 += nbytes;
	rndstats.ngetbytes1++;
    }
    else if( level >= 2 ) {
	rndstats.getbytes2 += nbytes;
	rndstats.ngetbytes2++;
    }

    buf = secure && secure_alloc ? xmalloc_secure( nbytes ) : xmalloc( nbytes );
    for( p = buf; nbytes > 0; ) {
	size_t n = nbytes > POOLSIZE? POOLSIZE : nbytes;
	read_pool( p, n, level );
	nbytes -= n;
	p += n;
    }
    return buf;
}