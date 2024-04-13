float timerend()
{
  LARGE_INTEGER unit, end;
  QueryPerformanceCounter(&end);
  QueryPerformanceFrequency(&unit);
  float msec = (float)(end.QuadPart - start.QuadPart);
  msec /= (float)unit.QuadPart / 1000.0f;
  return msec;
}