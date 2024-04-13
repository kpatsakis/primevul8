zlog_backtrace(int priority)
{
#ifndef HAVE_GLIBC_BACKTRACE
  zlog(NULL, priority, "No backtrace available on this platform.");
#else
  void *array[20];
  int size, i;
  char **strings;

  if (((size = backtrace(array,sizeof(array)/sizeof(array[0]))) <= 0) ||
      ((size_t)size > sizeof(array)/sizeof(array[0])))
    {
      zlog_err("Cannot get backtrace, returned invalid # of frames %d "
	       "(valid range is between 1 and %lu)",
	       size, (unsigned long)(sizeof(array)/sizeof(array[0])));
      return;
    }
  zlog(NULL, priority, "Backtrace for %d stack frames:", size);
  if (!(strings = backtrace_symbols(array, size)))
    {
      zlog_err("Cannot get backtrace symbols (out of memory?)");
      for (i = 0; i < size; i++)
	zlog(NULL, priority, "[bt %d] %p",i,array[i]);
    }
  else
    {
      for (i = 0; i < size; i++)
	zlog(NULL, priority, "[bt %d] %s",i,strings[i]);
      free(strings);
    }
#endif /* HAVE_GLIBC_BACKTRACE */
}