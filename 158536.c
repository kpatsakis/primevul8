string_timediff(struct timeval * diff)
{
static uschar buf[sizeof("0.000s")];

if (diff->tv_sec >= 5 || !LOGGING(millisec))
  return readconf_printtime((int)diff->tv_sec);

sprintf(CS buf, "%u.%03us", (uint)diff->tv_sec, (uint)diff->tv_usec/1000);
return buf;
}