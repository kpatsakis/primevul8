read_pool( byte *buffer, size_t length, int level )
{
    int i;
    ulong *sp, *dp;

    if( length > POOLSIZE ) {
	log_bug("too many random bits requested\n");
    }

    if( !pool_filled ) {
	if( read_seed_file() )
	    pool_filled = 1;
    }

    /* For level 2 quality (key generation) we alwas make
     * sure that the pool has been seeded enough initially */
    if( level == 2 && !did_initial_extra_seeding ) {
	size_t needed;

	pool_balance = 0;
	needed = length - pool_balance;
	if( needed < POOLSIZE/2 )
	    needed = POOLSIZE/2;
	else if( needed > POOLSIZE )
	    BUG();
	read_random_source( 3, needed, 2 );
	pool_balance += needed;
	did_initial_extra_seeding=1;
    }

    /* for level 2 make sure that there is enough random in the pool */
    if( level == 2 && pool_balance < length ) {
	size_t needed;

	if( pool_balance < 0 )
	    pool_balance = 0;
	needed = length - pool_balance;
	if( needed > POOLSIZE )
	    BUG();
	read_random_source( 3, needed, 2 );
	pool_balance += needed;
    }

    /* make sure the pool is filled */
    while( !pool_filled )
	random_poll();

    /* do always a fast random poll */
    fast_random_poll();

    if( !level ) { /* no need for cryptographic strong random */
	/* create a new pool */
	for(i=0,dp=(ulong*)keypool, sp=(ulong*)rndpool;
				    i < POOLWORDS; i++, dp++, sp++ )
	    *dp = *sp + ADD_VALUE;
	/* must mix both pools */
	mix_pool(rndpool); rndstats.mixrnd++;
	mix_pool(keypool); rndstats.mixkey++;
	memcpy( buffer, keypool, length );
    }
    else {
	/* mix the pool (if add_randomness() didn't it) */
	if( !just_mixed ) {
	    mix_pool(rndpool);
	    rndstats.mixrnd++;
	}
	/* create a new pool */
	for(i=0,dp=(ulong*)keypool, sp=(ulong*)rndpool;
				    i < POOLWORDS; i++, dp++, sp++ )
	    *dp = *sp + ADD_VALUE;
	/* and mix both pools */
	mix_pool(rndpool); rndstats.mixrnd++;
	mix_pool(keypool); rndstats.mixkey++;
	/* read the required data
	 * we use a readpoiter to read from a different postion each
	 * time */
	while( length-- ) {
	    *buffer++ = keypool[pool_readpos++];
	    if( pool_readpos >= POOLSIZE )
		pool_readpos = 0;
	    pool_balance--;
	}
	if( pool_balance < 0 )
	    pool_balance = 0;
	/* and clear the keypool */
	wipememory(keypool, POOLSIZE);
    }
}