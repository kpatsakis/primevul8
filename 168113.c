dirvote_get_start_of_next_interval(time_t now, int interval, int offset)
{
  struct tm tm;
  time_t midnight_today=0;
  time_t midnight_tomorrow;
  time_t next;

  tor_gmtime_r(&now, &tm);
  tm.tm_hour = 0;
  tm.tm_min = 0;
  tm.tm_sec = 0;

  if (tor_timegm(&tm, &midnight_today) < 0) {
    log_warn(LD_BUG, "Ran into an invalid time when trying to find midnight.");
  }
  midnight_tomorrow = midnight_today + (24*60*60);

  next = midnight_today + ((now-midnight_today)/interval + 1)*interval;

  /* Intervals never cross midnight. */
  if (next > midnight_tomorrow)
    next = midnight_tomorrow;

  /* If the interval would only last half as long as it's supposed to, then
   * skip over to the next day. */
  if (next + interval/2 > midnight_tomorrow)
    next = midnight_tomorrow;

  next += offset;
  if (next - interval > now)
    next -= interval;

  return next;
}