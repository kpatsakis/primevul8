static inline double ReadAccelerateTimer(AccelerateTimer *timer)
{
  return (double)timer->clocks/(double)timer->freq;
}