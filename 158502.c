write_syslog(int priority, const uschar *s)
{
int len, pass;
int linecount = 0;

if (!syslog_pid && LOGGING(pid))
  s = string_sprintf("%.*s%s", (int)pid_position[0], s, s + pid_position[1]);
if (!syslog_timestamp)
  {
  len = log_timezone ? 26 : 20;
  if (LOGGING(millisec)) len += 4;
  s += len;
  }

len = Ustrlen(s);

#ifndef NO_OPENLOG
if (!syslog_open && !f.running_in_test_harness)
  {
# ifdef SYSLOG_LOG_PID
  openlog(CS syslog_processname, LOG_PID|LOG_CONS, syslog_facility);
# else
  openlog(CS syslog_processname, LOG_CONS, syslog_facility);
# endif
  syslog_open = TRUE;
  }
#endif

/* First do a scan through the message in order to determine how many lines
it is going to end up as. Then rescan to output it. */

for (pass = 0; pass < 2; pass++)
  {
  int i;
  int tlen;
  const uschar * ss = s;
  for (i = 1, tlen = len; tlen > 0; i++)
    {
    int plen = tlen;
    uschar *nlptr = Ustrchr(ss, '\n');
    if (nlptr != NULL) plen = nlptr - ss;
#ifndef SYSLOG_LONG_LINES
    if (plen > MAX_SYSLOG_LEN) plen = MAX_SYSLOG_LEN;
#endif
    tlen -= plen;
    if (ss[plen] == '\n') tlen--;    /* chars left */

    if (pass == 0)
      linecount++;
    else if (f.running_in_test_harness)
      if (linecount == 1)
        fprintf(stderr, "SYSLOG: '%.*s'\n", plen, ss);
      else
        fprintf(stderr, "SYSLOG: '[%d%c%d] %.*s'\n", i,
          ss[plen] == '\n' && tlen != 0 ? '\\' : '/',
          linecount, plen, ss);
    else
      if (linecount == 1)
        syslog(priority, "%.*s", plen, ss);
      else
        syslog(priority, "[%d%c%d] %.*s", i,
          ss[plen] == '\n' && tlen != 0 ? '\\' : '/',
          linecount, plen, ss);

    ss += plen;
    if (*ss == '\n') ss++;
    }
  }
}