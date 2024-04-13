psf_rand_int32 (void)
{	static uint64_t value = 0 ;
	int k, count ;

	if (value == 0)
	{
#if HAVE_GETTIMEOFDAY
		struct timeval tv ;
		gettimeofday (&tv, NULL) ;
		value = tv.tv_sec + tv.tv_usec ;
#else
		value = time (NULL) ;
#endif
		} ;

	count = 4 + (value & 7) ;
	for (k = 0 ; k < count ; k++)
		value = (11117 * value + 211231) & 0x7fffffff ;

	return (int32_t) value ;
} /* psf_rand_int32 */