static inline void InitAccelerateTimer(AccelerateTimer *timer)
{
#ifdef _WIN32
  QueryPerformanceFrequency((LARGE_INTEGER*)&timer->freq);
#else
  timer->freq=(long long)1.0E3;
#endif
  timer->clocks=0;
  timer->start=0;
}