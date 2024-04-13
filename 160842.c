fast_random_poll()
{
    static int (*fnc)( void (*)(const void*, size_t, int), int) = NULL;
    static int initialized = 0;

    rndstats.fastpolls++;
    if( !initialized ) {
	if( !is_initialized )
	    initialize();
	initialized = 1;
	fnc = getfnc_fast_random_poll();
    }
    if( fnc ) {
	(*fnc)( add_randomness, 1 );
	return;
    }

    /* fall back to the generic function */
#if defined(HAVE_GETHRTIME) && !defined(HAVE_BROKEN_GETHRTIME)
    {	hrtime_t tv;
        /* On some Solaris and HPUX system gethrtime raises an SIGILL, but we
         * checked this with configure */
	tv = gethrtime();
	add_randomness( &tv, sizeof(tv), 1 );
    }
#elif defined (HAVE_GETTIMEOFDAY)
    {	struct timeval tv;
	if( gettimeofday( &tv, NULL ) )
	    BUG();
	add_randomness( &tv.tv_sec, sizeof(tv.tv_sec), 1 );
	add_randomness( &tv.tv_usec, sizeof(tv.tv_usec), 1 );
    }
#elif defined (HAVE_CLOCK_GETTIME)
    {	struct timespec tv;
	if( clock_gettime( CLOCK_REALTIME, &tv ) == -1 )
	    BUG();
	add_randomness( &tv.tv_sec, sizeof(tv.tv_sec), 1 );
	add_randomness( &tv.tv_nsec, sizeof(tv.tv_nsec), 1 );
    }
#elif defined (HAVE_TIMES)
    {	struct tms buf;
        if( times( &buf ) == -1 )
	    BUG();
	add_randomness( &buf, sizeof buf, 1 );
    }
#endif
#ifdef HAVE_GETRUSAGE
#ifndef RUSAGE_SELF
#ifdef __GCC__
#warning There is no RUSAGE_SELF on this system
#endif
#else
    {	struct rusage buf;
        /* QNX/Neutrino does return ENOSYS - so we just ignore it and
         * add whatever is in buf.  In a chroot environment it might not
         * work at all (i.e. because /proc/ is not accessible), so we better
         * ignore all error codes and hope for the best
         */
        getrusage( RUSAGE_SELF, &buf );

	add_randomness( &buf, sizeof buf, 1 );
	wipememory( &buf, sizeof buf );
    }
#endif
#endif
    /* time and clock are available on all systems - so
     * we better do it just in case one of the above functions
     * didn't work */
    {	time_t x = time(NULL);
	add_randomness( &x, sizeof(x), 1 );
    }
    {	clock_t x = clock();
	add_randomness( &x, sizeof(x), 1 );
    }
}