quagga_timestamp(int timestamp_precision, char *buf, size_t buflen)
{
  static struct {
    time_t last;
    size_t len;
    char buf[28];
  } cache;
  struct timeval clock;

  /* would it be sufficient to use global 'recent_time' here?  I fear not... */
  gettimeofday(&clock, NULL);

  /* first, we update the cache if the time has changed */
  if (cache.last != clock.tv_sec)
    {
      struct tm *tm;
      cache.last = clock.tv_sec;
      tm = localtime(&cache.last);
      cache.len = strftime(cache.buf, sizeof(cache.buf),
      			   "%Y/%m/%d %H:%M:%S", tm);
    }
  /* note: it's not worth caching the subsecond part, because
     chances are that back-to-back calls are not sufficiently close together
     for the clock not to have ticked forward */

  if (buflen > cache.len)
    {
      memcpy(buf, cache.buf, cache.len);
      if ((timestamp_precision > 0) &&
	  (buflen > cache.len+1+timestamp_precision))
	{
	  /* should we worry about locale issues? */
	  static const int divisor[] = {0, 100000, 10000, 1000, 100, 10, 1};
	  int prec;
	  char *p = buf+cache.len+1+(prec = timestamp_precision);
	  *p-- = '\0';
	  while (prec > 6)
	    /* this is unlikely to happen, but protect anyway */
	    {
	      *p-- = '0';
	      prec--;
	    }
	  clock.tv_usec /= divisor[prec];
	  do
	    {
	      *p-- = '0'+(clock.tv_usec % 10);
	      clock.tv_usec /= 10;
	    }
	  while (--prec > 0);
	  *p = '.';
	  return cache.len+1+timestamp_precision;
	}
      buf[cache.len] = '\0';
      return cache.len;
    }
  if (buflen > 0)
    buf[0] = '\0';
  return 0;
}