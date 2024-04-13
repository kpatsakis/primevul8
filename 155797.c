gather_time_entropy(void) {
#  ifdef _WIN32
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft); /* never fails */
  return ft.dwHighDateTime ^ ft.dwLowDateTime;
#  else
  struct timeval tv;
  int gettimeofday_res;

  gettimeofday_res = gettimeofday(&tv, NULL);

#    if defined(NDEBUG)
  (void)gettimeofday_res;
#    else
  assert(gettimeofday_res == 0);
#    endif /* defined(NDEBUG) */

  /* Microseconds time is <20 bits entropy */
  return tv.tv_usec;
#  endif
}