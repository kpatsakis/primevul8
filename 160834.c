add_randomness( const void *buffer, size_t length, int source )
{
    const byte *p = buffer;

    if( !is_initialized )
	initialize();
    rndstats.addbytes += length;
    rndstats.naddbytes++;
    while( length-- ) {
	rndpool[pool_writepos++] ^= *p++;
	if( pool_writepos >= POOLSIZE ) {
	    if( source > 1 )
		pool_filled = 1;
	    pool_writepos = 0;
	    mix_pool(rndpool); rndstats.mixrnd++;
	    just_mixed = !length;
	}
    }
}