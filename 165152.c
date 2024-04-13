static inline void StartAccelerateTimer(AccelerateTimer* timer)
{
#ifdef _WIN32
  QueryPerformanceCounter((LARGE_INTEGER*)&timer->start);
#else
  struct timeval
    s;
  gettimeofday(&s,0);
  timer->start=(long long)s.tv_sec*(long long)1.0E3+(long long)s.tv_usec/
    (long long)1.0E3;
#endif
}