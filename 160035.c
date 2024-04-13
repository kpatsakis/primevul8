void timerprint(const char *msg, const char *filename)
{
  gettimeofday(&end, NULL);
  float msec = (end.tv_sec - start.tv_sec) * 1000.0f +
               (end.tv_usec - start.tv_usec) / 1000.0f;
  printf("Timing: %s/%s: %6.3f msec\n", filename, msg, msec);
}