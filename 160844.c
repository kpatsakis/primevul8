quick_random_gen( int onoff )
{
    int last;

    read_random_source(0,0,0); /* init */
    last = quick_test;
    if( onoff != -1 )
	quick_test = onoff;
    return faked_rng? 1 : last;
}