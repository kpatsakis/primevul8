open_crashlog(void)
{
#define CRASHLOG_PREFIX "/var/tmp/quagga."
#define CRASHLOG_SUFFIX "crashlog"
  if (zlog_default && zlog_default->ident)
    {
      /* Avoid strlen since it is not async-signal-safe. */
      const char *p;
      size_t ilen;

      for (p = zlog_default->ident, ilen = 0; *p; p++)
	ilen++;
      {
	char buf[sizeof(CRASHLOG_PREFIX)+ilen+sizeof(CRASHLOG_SUFFIX)+3];
	char *s = buf;
#define LOC s,buf+sizeof(buf)-s
	s = str_append(LOC, CRASHLOG_PREFIX);
	s = str_append(LOC, zlog_default->ident);
	s = str_append(LOC, ".");
	s = str_append(LOC, CRASHLOG_SUFFIX);
#undef LOC
	*s = '\0';
	return open(buf, O_WRONLY|O_CREAT|O_EXCL, LOGFILE_MASK);
      }
    }
  return open(CRASHLOG_PREFIX CRASHLOG_SUFFIX, O_WRONLY|O_CREAT|O_EXCL,
	      LOGFILE_MASK);
#undef CRASHLOG_SUFFIX
#undef CRASHLOG_PREFIX
}