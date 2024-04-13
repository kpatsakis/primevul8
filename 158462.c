timesince(struct timeval * diff, struct timeval * then)
{
gettimeofday(diff, NULL);
diff->tv_sec -= then->tv_sec;
if ((diff->tv_usec -= then->tv_usec) < 0)
  {
  diff->tv_sec--;
  diff->tv_usec += 1000*1000;
  }
}