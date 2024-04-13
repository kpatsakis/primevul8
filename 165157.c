static inline void StopAccelerateTimer(AccelerateTimer *timer)
{
  long long
    n;

  n=0;
#ifdef _WIN32
  QueryPerformanceCounter((LARGE_INTEGER*)&(n));
#else
  struct timeval
    s;
  gettimeofday(&s,0);
  n=(long long)s.tv_sec*(long long)1.0E3+(long long)s.tv_usec/
    (long long)1.0E3;
#endif
  n-=timer->start;
  timer->start=0;
  timer->clocks+=n;
}