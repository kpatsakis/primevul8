zlog_backtrace_sigsafe(int priority, void *program_counter)
{
#ifdef HAVE_STACK_TRACE
  static const char pclabel[] = "Program counter: ";
  void *array[20];
  int size;
  char buf[100];
  char *s;
#define LOC s,buf+sizeof(buf)-s

#ifdef HAVE_GLIBC_BACKTRACE
  if (((size = backtrace(array,sizeof(array)/sizeof(array[0]))) <= 0) ||
      ((size_t)size > sizeof(array)/sizeof(array[0])))
    return;

#define DUMP(FD) { \
  if (program_counter) \
    { \
      write(FD, pclabel, sizeof(pclabel)-1); \
      backtrace_symbols_fd(&program_counter, 1, FD); \
    } \
  write(FD, buf, s-buf);	\
  backtrace_symbols_fd(array, size, FD); \
}
#elif defined(HAVE_PRINTSTACK)
#define DUMP(FD) { \
  if (program_counter) \
    write((FD), pclabel, sizeof(pclabel)-1); \
  write((FD), buf, s-buf); \
  printstack((FD)); \
}
#endif /* HAVE_GLIBC_BACKTRACE, HAVE_PRINTSTACK */

  s = buf;
  s = str_append(LOC,"Backtrace for ");
  s = num_append(LOC,size);
  s = str_append(LOC," stack frames:\n");

  if ((logfile_fd >= 0) || ((logfile_fd = open_crashlog()) >= 0))
    DUMP(logfile_fd)
  if (!zlog_default)
    DUMP(STDERR_FILENO)
  else
    {
      if (priority <= zlog_default->maxlvl[ZLOG_DEST_STDOUT])
	DUMP(STDOUT_FILENO)
      /* Remove trailing '\n' for monitor and syslog */
      *--s = '\0';
      if (priority <= zlog_default->maxlvl[ZLOG_DEST_MONITOR])
	vty_log_fixed(buf,s-buf);
      if (priority <= zlog_default->maxlvl[ZLOG_DEST_SYSLOG])
	syslog_sigsafe(priority|zlog_default->facility,buf,s-buf);
      {
	int i;
	/* Just print the function addresses. */
	for (i = 0; i < size; i++)
	  {
	    s = buf;
	    s = str_append(LOC,"[bt ");
	    s = num_append(LOC,i);
	    s = str_append(LOC,"] 0x");
	    s = hex_append(LOC,(u_long)(array[i]));
	    *s = '\0';
	    if (priority <= zlog_default->maxlvl[ZLOG_DEST_MONITOR])
	      vty_log_fixed(buf,s-buf);
	    if (priority <= zlog_default->maxlvl[ZLOG_DEST_SYSLOG])
	      syslog_sigsafe(priority|zlog_default->facility,buf,s-buf);
	  }
      }
    }
#undef DUMP
#undef LOC
#endif /* HAVE_STRACK_TRACE */
}