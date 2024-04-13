float timerend(void)
{
  gettimeofday(&end, NULL);
  float msec = (end.tv_sec - start.tv_sec) * 1000.0f +
               (end.tv_usec - start.tv_usec) / 1000.0f;
  return msec;
}