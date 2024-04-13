random_poll()
{
    rndstats.slowpolls++;
    read_random_source( 2, POOLSIZE/5, 1 );
}