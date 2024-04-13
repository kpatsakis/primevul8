read_random_source( int requester, size_t length, int level )
{
    static int (*fnc)(void (*)(const void*, size_t, int), int,
						    size_t, int) = NULL;
    if( !fnc ) {
	if( !is_initialized )
	    initialize();
	fnc = getfnc_gather_random();
	if( !fnc ) {
	    faked_rng = 1;
	    fnc = gather_faked;
	}
	if( !requester && !length && !level )
	    return; /* init only */
    }
    if( (*fnc)( add_randomness, requester, length, level ) < 0 )
	log_fatal("No way to gather entropy for the RNG\n");
}